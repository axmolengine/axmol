using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Text.RegularExpressions;
using ClangSharp.Interop;

namespace Axmol.LuaBindings;

/// <summary>
/// Collects the stable libclang C API surface needed by Axmol.
/// The generator intentionally does not instantiate ClangSharp's high-level AST
/// wrappers: those wrappers require libClangSharp, while Axmol's toolchain owns
/// and deploys libclang through 1kiss.
/// </summary>
public static unsafe class BindingGenerator
{
    private static IntPtr _libClangHandle;

    private static readonly string[] UnboundValueTypeNames =
    {
        "Acceleration",
        "ContactInfo2D",
        "SceneRenderState",
        "Animation3DData",
        "MaterialDatas",
        "NodeDatas",
        "SkinData",
        "VertexLayoutDesc",
        "VertexInputDesc"
    };

    private static readonly JsonSerializerOptions JsonOptions = new(JsonSerializerDefaults.Web)
    {
        WriteIndented = true,
        Converters = { new JsonStringEnumConverter() }
    };

    private sealed class CursorTree
    {
        public readonly Dictionary<CXCursor, List<CXCursor>> Children = new();
        public readonly List<CXCursor> Records = new();
        public readonly List<CXCursor> Enums = new();
        public readonly List<CXCursor> All = new();
        public readonly Dictionary<string, List<string>> TypeNames = new(StringComparer.Ordinal);
        public readonly Dictionary<string, List<string>> ValueNames = new(StringComparer.Ordinal);
        public readonly Dictionary<string, string> ValueExpressions = new(StringComparer.Ordinal);
        public readonly Dictionary<string, List<string>> RecordBases = new(StringComparer.Ordinal);
        public readonly Dictionary<string, string> TypeAliases = new(StringComparer.Ordinal);
        public readonly HashSet<string> DefinedTypes = new(StringComparer.Ordinal);

        public void Add(CXCursor cursor, CXCursor parent)
        {
            All.Add(cursor);
            if (!Children.TryGetValue(parent, out var children))
            {
                children = new List<CXCursor>();
                Children.Add(parent, children);
            }

            children.Add(cursor);
            switch (cursor.Kind)
            {
                case CXCursorKind.CXCursor_ClassDecl:
                case CXCursorKind.CXCursor_StructDecl:
                    Records.Add(cursor);
                    break;
                case CXCursorKind.CXCursor_EnumDecl:
                    Enums.Add(cursor);
                    break;
            }
        }

        public void BuildTypeIndex()
        {
            foreach (var cursor in All)
            {
                if (cursor.Kind is not (CXCursorKind.CXCursor_ClassDecl or
                    CXCursorKind.CXCursor_StructDecl or CXCursorKind.CXCursor_UnionDecl or
                    CXCursorKind.CXCursor_EnumDecl or CXCursorKind.CXCursor_TypedefDecl or
                    CXCursorKind.CXCursor_TypeAliasDecl))
                {
                    if (cursor.Kind == CXCursorKind.CXCursor_EnumConstantDecl ||
                        cursor.Kind == CXCursorKind.CXCursor_VarDecl)
                    {
                        var valueName = cursor.Spelling.CString;
                        var valueQualified = GetValueQualifiedName(cursor);
                        if (!string.IsNullOrWhiteSpace(valueName) && !string.IsNullOrWhiteSpace(valueQualified))
                        {
                            if (!ValueNames.TryGetValue(valueName, out var valueList))
                            {
                                valueList = new List<string>();
                                ValueNames.Add(valueName, valueList);
                            }
                            if (!valueList.Contains(valueQualified, StringComparer.Ordinal))
                                valueList.Add(valueQualified);
                            if (cursor.Kind == CXCursorKind.CXCursor_VarDecl)
                            {
                                var declaration = GetCursorText(cursor);
                                var equals = FindTopLevelEquals(declaration);
                                if (equals >= 0)
                                {
                                    var expression = declaration[(equals + 1)..].Trim().TrimEnd(';').Trim();
                                    if (expression.Length > 0 && IsSafeValueExpression(expression))
                                        ValueExpressions[valueName] = expression;
                                }
                            }
                        }
                    }
                    continue;
                }

                var name = cursor.Spelling.CString;
                if (string.IsNullOrWhiteSpace(name))
                    continue;
                var qualified = GetQualifiedName(cursor);
                if (string.IsNullOrWhiteSpace(qualified))
                    continue;
                if (cursor.Kind is CXCursorKind.CXCursor_TypedefDecl or CXCursorKind.CXCursor_TypeAliasDecl)
                {
                    var underlying = cursor.TypedefDeclUnderlyingType.Spelling.CString;
                    if (!string.IsNullOrWhiteSpace(underlying))
                        TypeAliases[qualified] = underlying;
                }
                if (IsDefinition(cursor))
                    DefinedTypes.Add(qualified);
                if (!TypeNames.TryGetValue(name, out var names))
                {
                    names = new List<string>();
                    TypeNames.Add(name, names);
                }
                if (!names.Contains(qualified, StringComparer.Ordinal))
                    names.Add(qualified);
            }

            foreach (var record in Records.Distinct())
            {
                // Forward declarations often appear after an included
                // definition.  They have no base specifiers and must not
                // erase the inheritance information collected from the
                // definition with the same qualified name.
                if (!IsDefinition(record))
                    continue;
                var recordName = GetQualifiedName(record);
                if (string.IsNullOrWhiteSpace(recordName))
                    continue;
                var bases = GetChildren(this, record)
                    .Where(x => x.Kind == CXCursorKind.CXCursor_CXXBaseSpecifier)
                    .Where(x => x.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic)
                    .Select(x => GetReferencedBaseName(x, this))
                    .Where(x => !string.IsNullOrWhiteSpace(x))
                    .Distinct(StringComparer.Ordinal)
                    .ToList();
                RecordBases[recordName] = bases;
            }
        }
    }

    private sealed class WalkState
    {
        public required CursorTree Tree { get; init; }
    }

    private static readonly CXCursorVisitor VisitCursor = VisitCursorCallback;

    /// <summary>
    /// Pins the libclang binary supplied by the host toolchain. Loading it
    /// explicitly is important on macOS: changing DYLD_LIBRARY_PATH after the
    /// PowerShell process has started does not reliably affect NativeLibrary.
    /// </summary>
    public static void ConfigureLibClang(string libraryPath)
    {
        if (string.IsNullOrWhiteSpace(libraryPath))
            throw new ArgumentException("A libclang library path is required.", nameof(libraryPath));
        if (!File.Exists(libraryPath))
            throw new FileNotFoundException("The libclang library was not found.", libraryPath);
        if (_libClangHandle != IntPtr.Zero)
            return;

        _libClangHandle = NativeLibrary.Load(libraryPath);
        if (_libClangHandle == IntPtr.Zero)
            throw new DllNotFoundException($"Unable to load libclang: {libraryPath}");

        clang.ResolveLibrary += (name, _, _) =>
            string.Equals(name, "libclang", StringComparison.Ordinal) ? _libClangHandle : IntPtr.Zero;
    }

    public static GenerationResult Generate(GenerationRequest request)
    {
        ArgumentNullException.ThrowIfNull(request);
        Directory.CreateDirectory(request.OutputDirectory);

        var diagnostics = new List<GenerationDiagnostic>();
        var classes = new List<BindingClass>();
        var enums = new List<BindingEnum>();
        var generatedFiles = new List<string>();
        // Validate configuration against the selected C++ declarations, not
        // only against emitted methods.  A skip often names an internal or
        // special-signature declaration which is intentionally filtered later.
        var availableMethods = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);

        using var index = CXIndex.Create();
        foreach (var header in request.Headers)
        {
            var fullHeader = Path.GetFullPath(Path.Combine(request.RepositoryRoot, header));
            if (!File.Exists(fullHeader))
            {
                diagnostics.Add(new GenerationDiagnostic
                {
                    Severity = "error",
                    Code = "AXLUA001",
                    Message = $"Header does not exist: {fullHeader}",
                    Header = fullHeader
                });
                continue;
            }

            var handle = CXTranslationUnit.Parse(
                index,
                fullHeader,
                request.ClangArguments.ToArray(),
                ReadOnlySpan<CXUnsavedFile>.Empty,
                // Match the historical clang.cindex parser.  Detailed
                // preprocessing records are not needed for binding emission
                // and make NDK arm_neon headers emit spurious builtin errors
                // with newer libclang runtimes.
                CXTranslationUnit_Flags.CXTranslationUnit_None);

            if (handle.Handle == IntPtr.Zero)
            {
                diagnostics.Add(new GenerationDiagnostic
                {
                    Severity = "error",
                    Code = "AXLUA003",
                    Message = $"Clang failed to create a translation unit: {fullHeader}",
                    Header = fullHeader
                });
                continue;
            }

            using (handle)
            {
                AddDiagnostics(handle, fullHeader, diagnostics);
                var tree = new CursorTree();
                var state = new WalkState { Tree = tree };
                var stateHandle = GCHandle.Alloc(state);
                try
                {
                    handle.Cursor.VisitChildren(VisitCursor, new CXClientData(GCHandle.ToIntPtr(stateHandle)));
                }
                finally
                {
                    stateHandle.Free();
                }
                tree.BuildTypeIndex();

                foreach (var record in tree.Records.Distinct())
                {
                    if (IsSelected(record, request) && IsDefinition(record))
                    {
                        var nativeName = record.Spelling.CString;
                        if (!availableMethods.TryGetValue(nativeName, out var methods))
                        {
                            methods = new HashSet<string>(StringComparer.Ordinal);
                            availableMethods.Add(nativeName, methods);
                        }
                        foreach (var method in GetChildren(tree, record).Where(x => x.Kind == CXCursorKind.CXCursor_CXXMethod))
                        {
                            var methodName = method.Spelling.CString;
                            if (!string.IsNullOrWhiteSpace(methodName))
                                methods.Add(methodName);
                        }
                        classes.Add(ToBindingClass(record, tree, request));
                    }
                }

                foreach (var enumeration in tree.Enums.Distinct())
                {
                    if (IsSelectedEnum(enumeration, request) && IsDefinition(enumeration))
                        enums.Add(ToBindingEnum(enumeration, tree));
                }
            }
        }

        var uniqueClasses = classes
            .GroupBy(x => x.QualifiedName, StringComparer.Ordinal)
            .Select(x => x.First())
            .OrderBy(x => x.QualifiedName, StringComparer.Ordinal)
            .ToArray();
        var uniqueEnums = enums
            .GroupBy(x => x.QualifiedName, StringComparer.Ordinal)
            .Select(x => x.First())
            .OrderBy(x => x.QualifiedName, StringComparer.Ordinal)
            .ToArray();

        // A typo in a skip rule silently leaves an implementation detail in
        // the public Lua surface.  Validate every rule against the selected
        // AST; feature-gated modules with no matching class remain valid.
        foreach (var rule in request.SkipRules)
        {
            var matchingClasses = availableMethods.Where(entry =>
                rule.ClassPattern == "*" || MatchesConfiguredPattern(entry.Key, rule.ClassPattern));
            if (rule.ClassPattern != "*" && !matchingClasses.Any())
                continue;
            foreach (var methodPattern in rule.MethodPatterns.Where(x => !string.IsNullOrWhiteSpace(x)))
            {
                var matched = matchingClasses.Any(entry => entry.Value.Any(methodName =>
                    methodPattern == "*" || MatchesConfiguredPrefix(methodName, methodPattern)));
                if (!matched)
                {
                    diagnostics.Add(new GenerationDiagnostic
                    {
                        Severity = "error",
                        Code = "AXLUA010",
                        Message = $"Skip rule '{rule.ClassPattern}:{methodPattern}' did not match a selected method."
                    });
                }
            }
        }

        foreach (var collision in uniqueClasses
                     .GroupBy(x => x.LuaClassName, StringComparer.Ordinal)
                     .Where(x => x.Select(bindingClass => bindingClass.QualifiedName)
                         .Distinct(StringComparer.Ordinal).Skip(1).Any()))
        {
            diagnostics.Add(new GenerationDiagnostic
            {
                Severity = "error",
                Code = "AXLUA004",
                Message = $"Multiple native classes map to Lua class '{request.LuaNamespace}.{collision.Key}': " +
                          string.Join(", ", collision.Select(x => x.QualifiedName).Distinct(StringComparer.Ordinal))
            });
        }

        // Keep generation transactional at the module level.  A translation
        // unit may still yield partial cursors after a Clang error, but those
        // cursors must never replace a previously valid generated module.
        if (diagnostics.Any(x => string.Equals(x.Severity, "error", StringComparison.OrdinalIgnoreCase)))
        {
            return new GenerationResult
            {
                Module = request.Module,
                ManifestPath = ManifestPathFor(request),
                GeneratedFiles = generatedFiles,
                Diagnostics = diagnostics,
                Classes = uniqueClasses,
                Enums = uniqueEnums
            };
        }

        var manifestPath = ManifestPathFor(request);
        var outputFiles = new List<(string Path, string Contents)>();
        if (request.EmitCpp)
        {
            outputFiles.Add((Path.Combine(request.OutputDirectory, $"axlua_{request.Module}_gen.h"),
                             CppEmitter.EmitHeader(request)));
            outputFiles.AddRange(CppEmitter.EmitSources(request, uniqueClasses, uniqueEnums)
                .Select(source => (Path.Combine(request.OutputDirectory, source.FileName), source.Contents)));
        }
        if (request.EmitManifest)
            outputFiles.Add((manifestPath, JsonSerializer.Serialize(new BindingManifest
            {
                Module = request.Module,
                RegistrationName = string.IsNullOrWhiteSpace(request.RegistrationName) ? request.Module : request.RegistrationName,
                LuaNamespace = request.LuaNamespace,
                LuaTypeNamespace = request.LuaTypeNamespace,
                ConditionalExpression = request.ConditionalExpression,
                Classes = uniqueClasses,
                Enums = uniqueEnums
            }, JsonOptions) + "\n"));

        CommitGeneratedFiles(outputFiles, generatedFiles);
        if (request.EmitCpp)
            RemoveStaleCppChunks(request, outputFiles.Select(x => x.Path));

        return new GenerationResult
        {
            Module = request.Module,
            ManifestPath = manifestPath,
            GeneratedFiles = generatedFiles,
            Diagnostics = diagnostics,
            Classes = uniqueClasses,
            Enums = uniqueEnums
        };
    }

    private static void RemoveStaleCppChunks(GenerationRequest request, IEnumerable<string> generatedPaths)
    {
        var retained = generatedPaths.Select(Path.GetFullPath).ToHashSet(StringComparer.OrdinalIgnoreCase);
        var prefix = $"axlua_{request.Module}_gen_";
        foreach (var path in Directory.GetFiles(request.OutputDirectory, $"{prefix}*.cpp"))
        {
            var fileName = Path.GetFileNameWithoutExtension(path);
            if (!fileName.StartsWith(prefix, StringComparison.Ordinal) ||
                !int.TryParse(fileName[prefix.Length..], out _))
                continue;
            if (!retained.Contains(Path.GetFullPath(path)))
                File.Delete(path);
        }
    }

    private static void CommitGeneratedFiles(
        IReadOnlyList<(string Path, string Contents)> files,
        ICollection<string> generatedFiles)
    {
        if (files.Count == 0)
            return;

        var staged = new List<(string TemporaryPath, string FinalPath)>();
        try
        {
            foreach (var file in files)
            {
                Directory.CreateDirectory(Path.GetDirectoryName(file.Path)!);
                var temporaryPath = $"{file.Path}.{Guid.NewGuid():N}.tmp";
                // StringBuilder.AppendLine follows Environment.NewLine.  The
                // generated sources are committed artifacts, so their bytes
                // must not vary between Windows and Unix hosts.
                var lfContents = file.Contents.Replace("\r\n", "\n", StringComparison.Ordinal)
                                              .Replace('\r', '\n');
                File.WriteAllText(temporaryPath, lfContents, new UTF8Encoding(false));
                staged.Add((temporaryPath, file.Path));
            }

            foreach (var file in staged)
            {
                File.Move(file.TemporaryPath, file.FinalPath, true);
                generatedFiles.Add(file.FinalPath);
            }
        }
        finally
        {
            foreach (var file in staged)
            {
                if (File.Exists(file.TemporaryPath))
                    File.Delete(file.TemporaryPath);
            }
        }
    }

    private static string ManifestPathFor(GenerationRequest request) =>
        Path.Combine(request.OutputDirectory, "manifests", $"axlua_{request.Module}.json");

    private static CXChildVisitResult VisitCursorCallback(CXCursor cursor, CXCursor parent, void* clientData)
    {
        try
        {
            var state = (WalkState?)GCHandle.FromIntPtr((IntPtr)clientData).Target;
            state?.Tree.Add(cursor, parent);
        }
        catch
        {
            return CXChildVisitResult.CXChildVisit_Break;
        }

        return CXChildVisitResult.CXChildVisit_Recurse;
    }

    private static void AddDiagnostics(
        CXTranslationUnit handle,
        string header,
        ICollection<GenerationDiagnostic> diagnostics)
    {
        for (var diagnosticIndex = 0u; diagnosticIndex < handle.NumDiagnostics; diagnosticIndex++)
        {
            using var diagnostic = handle.GetDiagnostic(diagnosticIndex);
            var severity = diagnostic.Severity;
            diagnostics.Add(new GenerationDiagnostic
            {
                Severity = severity >= CXDiagnosticSeverity.CXDiagnostic_Error
                    ? "error"
                    : severity.ToString().ToLowerInvariant(),
                Code = "AXLUA002",
                Message = diagnostic.Spelling.ToString(),
                Header = header
            });
        }
    }

    private static bool IsDefinition(CXCursor cursor) =>
        !cursor.IsNull && clang.isCursorDefinition(cursor) != 0;

    private static bool IsSelected(CXCursor cursor, GenerationRequest request)
    {
        if (cursor.IsNull || cursor.IsAnonymous || string.IsNullOrWhiteSpace(cursor.Spelling.CString))
            return false;

        // Keep the generated Lua surface aligned with the legacy tolua
        // bindings: declarations marked deprecated are not script APIs.  Do
        // this at the AST selection boundary so the rule applies equally to
        // direct and nested classes, regardless of the module configuration.
        if (IsDeprecated(cursor))
            return false;

        if (cursor.CXXAccessSpecifier is CX_CXXAccessSpecifier.CX_CXXProtected or
            CX_CXXAccessSpecifier.CX_CXXPrivate)
            return false;

        var name = cursor.Spelling.CString;
        if (UnboundValueTypeNames.Contains(name, StringComparer.Ordinal))
            return false;
        var qualifiedName = GetQualifiedName(cursor);
        if (qualifiedName.StartsWith("std::", StringComparison.Ordinal) ||
            qualifiedName.Contains("::__", StringComparison.Ordinal) ||
            name.StartsWith("__", StringComparison.Ordinal))
            return false;
        if (request.NativeNamespaces.Count > 0 &&
            !request.NativeNamespaces.Contains(GetNamespaceName(cursor), StringComparer.Ordinal))
            return false;
        return request.ClassPatterns.Count == 0 ||
               request.ClassPatterns.Any(pattern => MatchesConfiguredPattern(name, pattern));
    }

    private static bool IsSelectedEnum(CXCursor cursor, GenerationRequest request)
    {
        if (cursor.IsNull || cursor.IsAnonymous || string.IsNullOrWhiteSpace(cursor.Spelling.CString))
            return false;
        if (IsDeprecated(cursor))
            return false;
        if (request.NativeNamespaces.Count > 0 &&
            !request.NativeNamespaces.Contains(GetNamespaceName(cursor), StringComparer.Ordinal))
            return false;

        if (request.ClassPatterns.Count == 0 ||
            request.ClassPatterns.Any(pattern => MatchesConfiguredPattern(cursor.Spelling.CString, pattern)))
            return true;

        // A selected class owns its nested enums. Do not otherwise collect
        // every transitive ax:: enum from an umbrella header into each module.
        var owner = cursor.SemanticParent;
        return !owner.IsNull && request.ClassPatterns.Any(pattern =>
            MatchesConfiguredPattern(owner.Spelling.CString, pattern));
    }

    private static BindingClass ToBindingClass(CXCursor record, CursorTree tree, GenerationRequest request)
    {
        var nativeName = record.Spelling.CString;
        var qualifiedName = GetQualifiedName(record);
        var isAbstract = clang.CXXRecord_isAbstract(record) != 0;
        var noConstructors = request.NoConstructorPatterns.Any(pattern =>
            MatchesConfiguredPattern(nativeName, pattern));
        var children = GetChildren(tree, record);

        // Protected destructors are common on ref-counted Axmol types such as
        // Material. They remain valid non-owning userdata, but Lua must not
        // construct an instance whose lifetime it cannot end.
        var constructors = isAbstract || noConstructors || !HasBindableDestructor(record, tree)
            ? Array.Empty<BindingConstructor>()
            : children
                .Where(x => x.Kind == CXCursorKind.CXCursor_Constructor)
                .Where(IsUsableConstructor)
                .Where(x => IsSupportedSignature(x, tree))
                .Select(x => ToBindingConstructor(x, tree, qualifiedName))
                .GroupBy(x => string.Join(",", x.ParameterTypes), StringComparer.Ordinal)
                .Select(x => x.First())
                .ToArray();

        var directMethods = children
            .Where(x => x.Kind == CXCursorKind.CXCursor_CXXMethod)
            .Where(IsUsableMethod)
            .Where(x => IsSupportedSignature(x, tree))
            .Where(x => !x.Spelling.CString.StartsWith("operator", StringComparison.Ordinal))
            .Where(x => !IsSkipped(nativeName, x.Spelling.CString, request))
            .Select(x => ToBindingMethod(x, tree, nativeName, qualifiedName, request))
            .GroupBy(x => x.NativeName + "(" + string.Join(",", x.ParameterTypes) + ")" +
                          (x.IsConst ? " const" : string.Empty), StringComparer.Ordinal)
            .Select(x => x.First())
            .OrderBy(x => x.NativeName, StringComparer.Ordinal)
            .ThenBy(x => x.ParameterTypes.Count)
            .ToList();

        var fields = children
            .Where(x => x.Kind == CXCursorKind.CXCursor_FieldDecl)
            .Where(x => IsUsableField(x, tree))
            .Where(x => IsSelectedField(nativeName, x.Spelling.CString, request))
            .Select(x => new BindingField
            {
                NativeName = x.Spelling.CString,
                LuaName = x.Spelling.CString,
                FieldType = ResolveType(x.Type.Spelling.CString, qualifiedName, tree)
            })
            .GroupBy(x => x.NativeName, StringComparer.Ordinal)
            .Select(x => x.First())
            .OrderBy(x => x.NativeName, StringComparer.Ordinal)
            .ToArray();

        var noParent = request.NoParentPatterns.Any(pattern =>
            MatchesConfiguredPattern(nativeName, pattern));
        var directBases = noParent
            ? Array.Empty<string>()
            : children
                .Where(x => x.Kind == CXCursorKind.CXCursor_CXXBaseSpecifier)
                .Where(x => x.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic)
                .Select(x => GetBaseTypeName(x, record, tree))
                .Where(x => !string.IsNullOrWhiteSpace(x))
                .Where(x => !request.SkippedBasePatterns.Any(pattern =>
                    MatchesConfiguredPattern(x, pattern) ||
                    MatchesConfiguredPattern(x.Split("::").Last(), pattern)))
                .Distinct(StringComparer.Ordinal)
                .ToArray();
        var bases = ExpandBaseClosure(directBases, tree, request);

        // C++ overload lookup is name-based: a method declared by a derived
        // class hides all overloads with the same name in its bases unless
        // the derived declaration explicitly uses `using Base::method`.
        // Clang's C API exposes using declarations separately, so register
        // those imported base overloads in the same sol2 overload group.
        // Ordinary inherited methods remain available through the registered
        // sol2 base table and do not need duplicate registration.
        var importedNames = children
            .Where(x => x.Kind == CXCursorKind.CXCursor_UsingDeclaration)
            .Select(x => x.Spelling.CString)
            .Where(x => !string.IsNullOrWhiteSpace(x))
            .ToHashSet(StringComparer.Ordinal);
        foreach (var baseName in bases)
        {
            var baseRecord = tree.Records.FirstOrDefault(x =>
                IsDefinition(x) && string.Equals(GetQualifiedName(x), baseName, StringComparison.Ordinal));
            if (baseRecord.IsNull)
                continue;

            var baseNativeName = baseRecord.Spelling.CString;
            var baseMethods = GetChildren(tree, baseRecord)
                .Where(x => x.Kind == CXCursorKind.CXCursor_CXXMethod)
                .Where(x => importedNames.Contains(x.Spelling.CString))
                .Where(IsUsableMethod)
                .Where(x => IsSupportedSignature(x, tree))
                .Where(x => !x.Spelling.CString.StartsWith("operator", StringComparison.Ordinal))
                .Where(x => !IsSkipped(baseNativeName, x.Spelling.CString, request))
                .Select(x => ToBindingMethod(x, tree, baseNativeName, baseName, request));
            directMethods.AddRange(baseMethods);
        }

        var methods = directMethods
            .GroupBy(x => x.NativeName + "(" + string.Join(",", x.ParameterTypes) + ")" +
                          (x.IsConst ? " const" : string.Empty), StringComparer.Ordinal)
            .Select(x => x.First())
            .OrderBy(x => x.NativeName, StringComparer.Ordinal)
            .ThenBy(x => x.ParameterTypes.Count)
            .ToArray();

        return new BindingClass
        {
            NativeName = nativeName,
            QualifiedName = qualifiedName,
            LuaName = $"{request.LuaTypeNamespace}.{nativeName}",
            LuaClassName = RenameClass(nativeName, request),
            IsAbstract = isAbstract,
            Constructors = constructors,
            Bases = bases,
            Methods = methods,
            Fields = fields
        };
    }

    private static IReadOnlyList<string> ExpandBaseClosure(
        IReadOnlyList<string> directBases,
        CursorTree tree,
        GenerationRequest request)
    {
        var order = new List<string>();
        var pathCounts = new Dictionary<string, int>(StringComparer.Ordinal);

        void VisitBase(string baseName, HashSet<string> currentPath)
        {
            if (string.IsNullOrWhiteSpace(baseName) ||
                request.SkippedBasePatterns.Any(pattern =>
                    MatchesConfiguredPattern(baseName, pattern) ||
                    MatchesConfiguredPattern(baseName.Split("::").Last(), pattern)) ||
                !currentPath.Add(baseName))
            {
                return;
            }

            if (!pathCounts.TryGetValue(baseName, out var pathCount))
            {
                order.Add(baseName);
                pathCount = 0;
            }
            // Two paths are enough to prove that a C++ derived-to-base cast
            // may be ambiguous.  Capping the count also keeps large external
            // inheritance graphs cheap to inspect.
            pathCounts[baseName] = Math.Min(pathCount + 1, 2);

            if (tree.RecordBases.TryGetValue(baseName, out var ancestorBases))
            {
                foreach (var ancestorBase in ancestorBases)
                    VisitBase(ancestorBase, currentPath);
            }
            currentPath.Remove(baseName);
        }

        foreach (var directBase in directBases)
            VisitBase(directBase, new HashSet<string>(StringComparer.Ordinal));

        return order.Where(baseName => pathCounts[baseName] == 1).ToArray();
    }

    private static BindingConstructor ToBindingConstructor(CXCursor cursor, CursorTree tree, string contextType)
    {
        var parameters = GetParameters(cursor, tree);
        return new BindingConstructor
        {
            ParameterTypes = parameters.Select(x => ResolveType(x.Type.Spelling.CString, contextType, tree)).ToArray(),
            DefaultValues = parameters.Select(x => GetDefaultValue(x, contextType, tree)).ToArray()
        };
    }

    private static BindingMethod ToBindingMethod(
        CXCursor cursor,
        CursorTree tree,
        string className,
        string contextType,
        GenerationRequest request)
    {
        var parameters = GetParameters(cursor, tree);
        return new BindingMethod
        {
            NativeName = cursor.Spelling.CString,
            LuaName = RenameMethod(className, cursor.Spelling.CString, request),
            DeclaringType = contextType,
            ReturnType = ResolveType(cursor.ResultType.Spelling.CString, contextType, tree),
            ParameterTypes = parameters.Select(x => ResolveType(x.Type.Spelling.CString, contextType, tree)).ToArray(),
            DefaultValues = parameters.Select(x => GetDefaultValue(x, contextType, tree)).ToArray(),
            IsStatic = clang.CXXMethod_isStatic(cursor) != 0,
            IsConst = clang.CXXMethod_isConst(cursor) != 0,
            IsVariadic = clang.Cursor_isVariadic(cursor) != 0
        };
    }

    private static bool IsUsableConstructor(CXCursor cursor) =>
        !IsDeprecated(cursor) &&
        cursor.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic &&
        clang.CXXMethod_isDeleted(cursor) == 0 &&
        clang.CXXConstructor_isCopyConstructor(cursor) == 0 &&
        clang.CXXConstructor_isMoveConstructor(cursor) == 0 &&
        clang.Cursor_isVariadic(cursor) == 0;

    private static bool IsUsableMethod(CXCursor cursor) =>
        !IsDeprecated(cursor) &&
        cursor.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic &&
        clang.CXXMethod_isDeleted(cursor) == 0 &&
        clang.Cursor_isVariadic(cursor) == 0 &&
        !Regex.IsMatch(GetCursorText(cursor), @"^\s*\[\[\s*internal\s*\]\]", RegexOptions.CultureInvariant);

    private static bool IsUsableField(CXCursor cursor, CursorTree tree)
    {
        if (IsDeprecated(cursor) ||
            cursor.CXXAccessSpecifier != CX_CXXAccessSpecifier.CX_CXXPublic ||
            cursor.IsAnonymous || string.IsNullOrWhiteSpace(cursor.Spelling.CString))
            return false;

        var type = ResolveType(cursor.Type.Spelling.CString,
                               GetQualifiedName(clang.getCursorSemanticParent(cursor)), tree).Trim();
        // Callback members deliberately use pointer parameters for engine objects
        // (for example std::function<void(PointerEvent*)>).  They are safe to
        // export when explicitly selected by a field rule: sol converts the Lua
        // callable to std::function and the pointee remains an engine-owned,
        // separately bound type.  Keep the general raw-pointer rejection below
        // for ordinary data members.
        var isFunction = type.StartsWith("std::function<", StringComparison.Ordinal);
        return !type.StartsWith("const ", StringComparison.Ordinal) &&
               (isFunction || !type.Contains('*')) &&
               !type.Contains('[') &&
               !type.Contains("std::unique_ptr", StringComparison.Ordinal) &&
               !type.Contains("std::span", StringComparison.Ordinal) &&
               !HasIncompleteNamedType(type, GetQualifiedName(clang.getCursorSemanticParent(cursor)), tree);
    }

    private static bool IsSelectedField(string className, string fieldName, GenerationRequest request) =>
        request.FieldRules.Any(rule =>
            (rule.ClassPattern == "*" || MatchesConfiguredPattern(className, rule.ClassPattern)) &&
            rule.FieldPatterns.Any(pattern => MatchesConfiguredPattern(fieldName, pattern)));

    private static bool IsSupportedSignature(CXCursor cursor, CursorTree tree)
    {
        var context = GetQualifiedName(clang.getCursorSemanticParent(cursor));
        var types = GetParameters(cursor, tree)
            .Select(x => ExpandCallableAliases(x.Type.Spelling.CString, tree))
            .Append(cursor.Kind == CXCursorKind.CXCursor_Constructor
                ? string.Empty
                : ExpandCallableAliases(cursor.ResultType.Spelling.CString, tree));
        return types.All(type =>
            // A variadic C ABI parameter has no meaningful Lua equivalent;
            // emitting it produces a method that compiles but can never be
            // called safely from Lua (for example MenuItemToggle's va_list
            // overload).
            !type.Contains("va_list", StringComparison.Ordinal) &&
            !type.Contains("ax::Coroutine", StringComparison.Ordinal) &&
            !type.Contains("std::unordered_map", StringComparison.Ordinal) &&
            !type.Contains("tsl::robin_map", StringComparison.Ordinal) &&
            !type.Contains("tlx::string_map", StringComparison.Ordinal) &&
            !type.Contains("ValueMap", StringComparison.Ordinal) &&
            !type.Contains("std::span", StringComparison.Ordinal) &&
            !type.Contains("std::unique_ptr", StringComparison.Ordinal) &&
            // An erased native pointer has no stable Lua representation or
            // ownership contract.  It must use a dedicated adapter/table
            // converter, never sol2's light/userdata fallback.
            !ContainsOpaquePointer(type) &&
            // Scalar out-parameters are an implementation detail of the C++
            // API.  Lua has no addressable float*/int* value, and sol2 would
            // otherwise emit a callable that can never be invoked correctly.
            !ContainsScalarPointer(type) &&
            // Cocos-era selector aliases are member-function pointers, not
            // Lua-callable std::function parameters.  Keep them out of an
            // overload group so a modern lambda overload remains bindable.
            !Regex.IsMatch(type, @"\b(?:ax::)?SEL_[A-Za-z_]\w*\b", RegexOptions.CultureInvariant) &&
            !type.Contains("GLFWwindow", StringComparison.Ordinal) &&
            !type.Contains("VulkanInterop", StringComparison.Ordinal) &&
            !type.Contains("JPH::", StringComparison.Ordinal) &&
            !type.Contains("&&", StringComparison.Ordinal) &&
            !ContainsUnboundValueType(type) &&
            !HasIncompleteNamedType(type, context, tree));
    }

    // Keep this list explicit.  These are native implementation/data records
    // for which Axmol has no registered table converter or generated value
    // type.  They must not silently fall through to sol2's generic userdata
    // path, which changes the historical Lua ABI.  Add a converter or a
    // configuration skip before removing an entry.
    private static bool ContainsUnboundValueType(string type)
    {
        return UnboundValueTypeNames.Any(name => Regex.IsMatch(
            type,
            $@"(?<![A-Za-z0-9_])(?:[A-Za-z_][A-Za-z0-9_]*::)*{Regex.Escape(name)}(?=\s*(?:[&*]|$))",
            RegexOptions.CultureInvariant));
    }

    private static bool ContainsOpaquePointer(string type) =>
        Regex.IsMatch(type, @"(?<![A-Za-z0-9_])(?:const\s+)?void\s*\*", RegexOptions.CultureInvariant);

    private static bool ContainsScalarPointer(string type) =>
        Regex.IsMatch(type,
            @"(?<![A-Za-z0-9_])(?:const\s+)?(?:bool|char|short|int|long|long\s+long|float|double|size_t|uint\d*_t|int\d*_t)\s*\*",
            RegexOptions.CultureInvariant);

    private static bool HasIncompleteNamedType(string type, string context, CursorTree tree)
    {
        var resolved = ResolveType(type, context, tree);
        foreach (Match match in Regex.Matches(resolved, @"(?:[A-Za-z_]\w*::)*[A-Za-z_]\w*"))
        {
            var qualifiedToken = match.Value;
            var shortName = qualifiedToken[(qualifiedToken.LastIndexOf("::", StringComparison.Ordinal) + 2)..];
            if (!tree.TypeNames.TryGetValue(shortName, out var candidates))
                continue;
            if (candidates.Any(candidate =>
                    string.Equals(candidate, qualifiedToken, StringComparison.Ordinal) &&
                    !tree.DefinedTypes.Contains(candidate)))
                return true;
        }
        return false;
    }

    private static bool HasBindableDestructor(CXCursor record, CursorTree tree) =>
        GetChildren(tree, record)
            .Where(x => x.Kind == CXCursorKind.CXCursor_Destructor)
            .All(x => x.CXXAccessSpecifier == CX_CXXAccessSpecifier.CX_CXXPublic);

    private static IReadOnlyList<CXCursor> GetParameters(CXCursor cursor, CursorTree tree) =>
        GetChildren(tree, cursor)
            .Where(x => x.Kind == CXCursorKind.CXCursor_ParmDecl)
            .OrderBy(GetSourceOffset)
            .ToArray();

    private static string ResolveType(string type, string contextType, CursorTree tree)
    {
        if (string.IsNullOrWhiteSpace(type) || tree.TypeNames.Count == 0)
            return type;

        var contextParts = contextType.Split("::", StringSplitOptions.RemoveEmptyEntries).ToList();
        var matches = Regex.Matches(type, @"[A-Za-z_]\w*");
        if (matches.Count == 0)
            return type;

        var result = new StringBuilder(type.Length + 16);
        var position = 0;
        foreach (Match match in matches)
        {
            result.Append(type, position, match.Index - position);
            var token = match.Value;
            var before = match.Index > 0 ? type[match.Index - 1] : '\0';
            var afterIndex = match.Index + match.Length;
            var replacement = token;
            var isNumericSuffix = match.Index > 0 &&
                                  (char.IsDigit(before) || before == '.') &&
                                  (token is "f" or "F" or "l" or "L" or "u" or "U" or "ll" or "LL");
            if (before != ':' && !isNumericSuffix && !IsBuiltinTypeToken(token))
            {
                replacement = FindQualifiedType(token, contextParts, tree) ?? token;
            }
            result.Append(replacement);
            position = afterIndex;
        }
        result.Append(type, position, type.Length - position);
        return result.ToString();
    }

    private static string ExpandCallableAliases(string type, CursorTree tree)
    {
        if (string.IsNullOrWhiteSpace(type) || tree.TypeAliases.Count == 0)
            return type;

        var expanded = type;
        for (var pass = 0; pass < 8; pass++)
        {
            var changed = false;
            foreach (var alias in tree.TypeAliases.OrderByDescending(x => x.Key.Length))
            {
                if (!alias.Value.Contains("std::function", StringComparison.Ordinal))
                    continue;
                var pattern = $"(?<![A-Za-z0-9_:]){Regex.Escape(alias.Key)}(?![A-Za-z0-9_:])";
                var replacement = Regex.Replace(expanded, pattern, alias.Value);
                if (!string.Equals(replacement, expanded, StringComparison.Ordinal))
                {
                    expanded = replacement;
                    changed = true;
                }
            }
            if (!changed)
                break;
        }
        return expanded;
    }

    private static string? FindQualifiedType(
        string token,
        IReadOnlyList<string> contextParts,
        CursorTree tree)
    {
        if (!tree.TypeNames.TryGetValue(token, out var candidates))
            return null;

        for (var length = contextParts.Count; length > 0; length--)
        {
            var prefix = string.Join("::", contextParts.Take(length));
            var nested = prefix + "::" + token;
            var exact = candidates.FirstOrDefault(x => string.Equals(x, nested, StringComparison.Ordinal));
            if (exact is not null)
                return exact;
        }

        var axType = candidates.FirstOrDefault(x => string.Equals(x, "ax::" + token, StringComparison.Ordinal));
        if (axType is not null)
            return axType;

        var context = string.Join("::", contextParts);
        var inherited = candidates.FirstOrDefault(candidate =>
            IsDerivedFrom(context, GetOwnerName(candidate), tree, new HashSet<string>(StringComparer.Ordinal)));
        return inherited ?? (candidates.Count == 1 ? candidates[0] : null);
    }

    private static string ResolveExpression(string expression, string contextType, CursorTree tree)
    {
        var contextParts = contextType.Split("::", StringSplitOptions.RemoveEmptyEntries).ToList();
        var matches = Regex.Matches(expression, @"[A-Za-z_]\w*");
        if (matches.Count == 0)
            return expression;

        var result = new StringBuilder(expression.Length + 16);
        var position = 0;
        foreach (Match match in matches)
        {
            result.Append(expression, position, match.Index - position);
            var token = match.Value;
            var before = match.Index > 0 ? expression[match.Index - 1] : '\0';
            var afterIndex = match.Index + match.Length;
            var replacement = token;
            var isNumericSuffix = match.Index > 0 &&
                                  (char.IsDigit(before) || before == '.') &&
                                  (token is "f" or "F" or "l" or "L" or "u" or "U" or "ll" or "LL");
            if (before != ':' && !isNumericSuffix && !IsBuiltinTypeToken(token))
            {
                replacement = FindQualifiedType(token, contextParts, tree) ??
                              (tree.ValueExpressions.TryGetValue(token, out var valueExpression)
                                  ? valueExpression
                                  : FindQualifiedValue(token, contextParts, tree.ValueNames) ?? token);
            }
            result.Append(replacement);
            position = afterIndex;
        }
        result.Append(expression, position, expression.Length - position);
        return result.ToString();
    }

    private static string? FindQualifiedValue(
        string token,
        IReadOnlyList<string> contextParts,
        IReadOnlyDictionary<string, List<string>> valueNames)
    {
        if (!valueNames.TryGetValue(token, out var candidates))
            return null;

        for (var length = contextParts.Count; length > 0; length--)
        {
            var prefix = string.Join("::", contextParts.Take(length));
            var exact = candidates.FirstOrDefault(x =>
                x.StartsWith(prefix + "::", StringComparison.Ordinal));
            if (exact is not null)
                return exact;
        }

        return candidates.Count == 1 ? candidates[0] : null;
    }

    private static string GetOwnerName(string qualifiedName)
    {
        var separator = qualifiedName.LastIndexOf("::", StringComparison.Ordinal);
        return separator < 0 ? string.Empty : qualifiedName[..separator];
    }

    private static bool IsDerivedFrom(
        string typeName,
        string possibleBase,
        CursorTree tree,
        HashSet<string> visited)
    {
        if (string.Equals(typeName, possibleBase, StringComparison.Ordinal))
            return true;
        if (!visited.Add(typeName) || !tree.RecordBases.TryGetValue(typeName, out var bases))
            return false;

        return bases.Any(baseName =>
            string.Equals(baseName, possibleBase, StringComparison.Ordinal) ||
            IsDerivedFrom(baseName, possibleBase, tree, visited));
    }

    private static bool IsBuiltinTypeToken(string token) => token switch
    {
        "alignas" or "auto" or "bool" or "char" or "char8_t" or "char16_t" or "char32_t" or
        "class" or "const" or "constexpr" or "decltype" or "double" or "enum" or "explicit" or
        "float" or "inline" or "int" or "long" or "mutable" or "noexcept" or "nullptr_t" or
        "register" or "short" or "signed" or "static" or "struct" or "template" or "thread_local" or
        "typename" or "union" or "unsigned" or "void" or "volatile" or "wchar_t" or "using" => true,
        _ => false
    };

    private static bool IsSafeValueExpression(string expression) =>
        Regex.IsMatch(expression,
            @"^(?:[-+]?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?[fFlLuU]*|true|false|nullptr)$");

    private static string GetBaseTypeName(CXCursor baseSpecifier, CXCursor derived, CursorTree tree)
    {
        var typeName = baseSpecifier.Type.Spelling.CString;
        var typeReference = GetChildren(tree, baseSpecifier)
            .FirstOrDefault(x => x.Kind == CXCursorKind.CXCursor_TypeRef);
        if (!typeReference.IsNull)
        {
            var referenced = clang.getCursorReferenced(typeReference);
            if (!referenced.IsNull && !string.IsNullOrWhiteSpace(referenced.Spelling.CString))
                return GetQualifiedName(referenced);
        }

        if (typeName.Contains("::", StringComparison.Ordinal))
            return typeName;

        var qualifiedDerived = GetQualifiedName(derived);
        var separator = qualifiedDerived.LastIndexOf("::", StringComparison.Ordinal);
        return separator < 0 ? typeName : qualifiedDerived[..separator] + "::" + typeName;
    }

    private static string GetReferencedBaseName(CXCursor baseSpecifier, CursorTree tree)
    {
        var typeReference = GetChildren(tree, baseSpecifier)
            .FirstOrDefault(x => x.Kind == CXCursorKind.CXCursor_TypeRef);
        if (!typeReference.IsNull)
        {
            var referenced = clang.getCursorReferenced(typeReference);
            if (!referenced.IsNull)
                return GetQualifiedName(referenced);
        }
        return baseSpecifier.Type.Spelling.CString;
    }

    private static string? GetDefaultValue(CXCursor parameter, string contextType, CursorTree tree)
    {
        var text = GetCursorText(parameter);
        var equals = FindTopLevelEquals(text);
        if (equals < 0)
            return null;

        var value = text[(equals + 1)..].Trim();
        return value.Length == 0 ? null : ResolveExpression(value, contextType, tree);
    }

    private static string GetCursorText(CXCursor cursor)
    {
        var start = cursor.Extent.Start;
        var end = cursor.Extent.End;
        start.GetFileLocation(out _, out _, out _, out var startOffset);
        end.GetFileLocation(out _, out _, out _, out var endOffset);
        if (endOffset <= startOffset)
            return string.Empty;

        cursor.Location.GetPresumedLocation(out var fileName, out _, out _);
        using (fileName)
        {
            var path = fileName.CString;
            if (string.IsNullOrWhiteSpace(path) || !File.Exists(path))
                return string.Empty;

            var bytes = File.ReadAllBytes(path);
            var count = (int)Math.Min(endOffset - startOffset, (uint)Math.Max(0, bytes.Length - startOffset));
            return count <= 0 ? string.Empty : Encoding.UTF8.GetString(bytes, (int)startOffset, count);
        }
    }

    private static int GetSourceOffset(CXCursor cursor)
    {
        cursor.Location.GetFileLocation(out _, out _, out _, out var offset);
        return offset > int.MaxValue ? int.MaxValue : (int)offset;
    }

    private static int FindTopLevelEquals(string text)
    {
        var angle = 0;
        var paren = 0;
        var bracket = 0;
        var brace = 0;
        var quote = '\0';
        for (var i = 0; i < text.Length; i++)
        {
            var c = text[i];
            if (quote != '\0')
            {
                if (c == quote && (i == 0 || text[i - 1] != '\\'))
                    quote = '\0';
                continue;
            }

            if (c is '\'' or '"')
            {
                quote = c;
                continue;
            }

            switch (c)
            {
                case '<': angle++; break;
                case '>': angle = Math.Max(0, angle - 1); break;
                case '(': paren++; break;
                case ')': paren = Math.Max(0, paren - 1); break;
                case '[': bracket++; break;
                case ']': bracket = Math.Max(0, bracket - 1); break;
                case '{': brace++; break;
                case '}': brace = Math.Max(0, brace - 1); break;
                case '=' when angle == 0 && paren == 0 && bracket == 0 && brace == 0:
                    if ((i == 0 || text[i - 1] != '=') && (i + 1 == text.Length || text[i + 1] != '='))
                        return i;
                    break;
            }
        }

        return -1;
    }

    private static BindingEnum ToBindingEnum(CXCursor enumeration, CursorTree tree)
    {
        var owner = enumeration.SemanticParent;
        return new BindingEnum
        {
            NativeName = enumeration.Spelling.CString,
            QualifiedName = GetQualifiedName(enumeration),
            OwnerQualifiedName = owner.Kind is CXCursorKind.CXCursor_ClassDecl or CXCursorKind.CXCursor_StructDecl
                ? GetQualifiedName(owner)
                : null,
            Values = GetChildren(tree, enumeration)
                .Where(x => x.Kind == CXCursorKind.CXCursor_EnumConstantDecl)
                .Where(x => !IsDeprecated(x))
                .Select(x => new BindingEnumValue
                {
                    Name = x.Spelling.CString,
                    SignedValue = clang.getEnumConstantDeclValue(x),
                    UnsignedValue = clang.getEnumConstantDeclUnsignedValue(x),
                    IsUnsigned = clang.getEnumConstantDeclUnsignedValue(x) > long.MaxValue
                })
                .ToArray()
        };
    }

    private static bool IsDeprecated(CXCursor cursor) =>
        clang.getCursorAvailability(cursor) == CXAvailabilityKind.CXAvailability_Deprecated;

    private static IReadOnlyList<CXCursor> GetChildren(CursorTree tree, CXCursor cursor) =>
        tree.Children.TryGetValue(cursor, out var children) ? children : Array.Empty<CXCursor>();

    private static string GetQualifiedName(CXCursor cursor)
    {
        var parts = new List<string>();
        var current = cursor;
        var guard = 0;
        while (!current.IsNull && guard++ < 64)
        {
            var spelling = current.Spelling.CString;
            if (!string.IsNullOrWhiteSpace(spelling) &&
                current.Kind != CXCursorKind.CXCursor_TranslationUnit)
                parts.Add(spelling);

            var parent = current.SemanticParent;
            if (parent.IsNull || parent.Equals(current))
                break;
            current = parent;
        }

        parts.Reverse();
        return string.Join("::", parts);
    }

    private static string GetValueQualifiedName(CXCursor cursor)
    {
        var valueName = cursor.Spelling.CString;
        var enumParent = cursor.SemanticParent;
        if (enumParent.IsNull)
            return GetQualifiedName(cursor);

        var enumName = enumParent.Spelling.CString;
        if (!string.IsNullOrWhiteSpace(enumName) &&
            !enumName.StartsWith("(unnamed", StringComparison.Ordinal))
            return GetQualifiedName(enumParent) + "::" + valueName;

        var owner = enumParent.SemanticParent;
        var ownerName = GetQualifiedName(owner);
        return string.IsNullOrWhiteSpace(ownerName) ? valueName : ownerName + "::" + valueName;
    }

    private static string GetNamespaceName(CXCursor cursor)
    {
        var parts = new List<string>();
        var current = cursor.SemanticParent;
        while (!current.IsNull)
        {
            if (current.Kind == CXCursorKind.CXCursor_Namespace &&
                !string.IsNullOrWhiteSpace(current.Spelling.CString))
                parts.Add(current.Spelling.CString);

            var parent = current.SemanticParent;
            if (parent.IsNull || parent.Equals(current))
                break;
            current = parent;
        }

        parts.Reverse();
        return string.Join("::", parts);
    }

    private static string RenameClass(string nativeName, GenerationRequest request) =>
        request.ClassRenames.FirstOrDefault(x => string.Equals(x.NativeName, nativeName, StringComparison.Ordinal))?.LuaName ?? nativeName;

    private static string RenameMethod(string className, string methodName, GenerationRequest request)
    {
        var rule = request.RenameRules.FirstOrDefault(x =>
            (x.ClassPattern == "*" || MatchesConfiguredPattern(className, x.ClassPattern)) &&
            MatchesConfiguredPrefix(methodName, x.MethodPattern));
        return rule?.LuaName ?? methodName;
    }

    private static bool IsSkipped(string className, string methodName, GenerationRequest request)
    {
        if (request.SkipPatterns.Any(pattern => MatchesConfiguredPrefix(methodName, pattern)))
            return true;

        return request.SkipRules.Any(rule =>
            (rule.ClassPattern == "*" || MatchesConfiguredPattern(className, rule.ClassPattern)) &&
            rule.MethodPatterns.Any(pattern =>
                !string.IsNullOrWhiteSpace(pattern) &&
                (pattern == "*" || MatchesConfiguredPrefix(methodName, pattern))));
    }

    private static bool MatchesConfiguredPattern(string value, string pattern) =>
        Regex.IsMatch(value, "^(?:" + pattern + ")$", RegexOptions.CultureInvariant);

    private static bool MatchesConfiguredPrefix(string value, string pattern) =>
        Regex.IsMatch(value, "^(?:" + pattern + ")", RegexOptions.CultureInvariant);
}
