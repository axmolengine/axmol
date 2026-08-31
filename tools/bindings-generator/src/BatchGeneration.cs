using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.Json;
using System.Text.RegularExpressions;

namespace Axmol.LuaBindings;

/// <summary>
/// Inputs which are independent of the host toolchain.  PowerShell prepares
/// libclang, Android includes, and the base clang arguments; the C# layer owns
/// module discovery, configuration parsing, policy validation, and generation.
/// </summary>
public sealed class BatchGenerationRequest
{
    public string RepositoryRoot { get; set; } = string.Empty;
    public string ConfigurationDirectory { get; set; } = string.Empty;
    public string OutputDirectory { get; set; } = string.Empty;
    public string AdapterAllowlistPath { get; set; } = string.Empty;
    public string Module { get; set; } = "all";
    public IReadOnlyList<string> ClangArguments { get; set; } = Array.Empty<string>();
    public bool Verify { get; set; }
}

public sealed class BatchGenerationResult
{
    public required IReadOnlyList<GenerationResult> Results { get; init; }
    public required IReadOnlyList<GenerationDiagnostic> Diagnostics { get; init; }
}

/// <summary>
/// Loads the checked-in module JSON and coordinates a complete generation run.
/// Keeping this in the managed generator prevents PowerShell and C# from
/// maintaining two subtly different configuration implementations.
/// </summary>
public static class BatchBindingGenerator
{
    private static readonly HashSet<string> ConfigProperties = new(StringComparer.Ordinal)
    {
        "module", "target_namespace", "cpp_namespace", "macro_judgement", "headers", "clang_flags",
        "cpp_chunks", "classes", "abstract_classes", "no_constructors", "classes_have_no_parents",
        "base_classes_to_skip", "fields", "skip", "rename_functions", "rename_classes", "fast_bindings"
    };

    public static BatchGenerationResult Generate(BatchGenerationRequest request)
    {
        ArgumentNullException.ThrowIfNull(request);
        var diagnostics = new List<GenerationDiagnostic>();
        var results = new List<GenerationResult>();

        ValidateRequest(request);
        ValidateManualAdapters(request);

        var configPaths = SelectConfigurations(request.ConfigurationDirectory, request.Module);
        var registeredClasses = new Dictionary<string, (string Module, string NativeName)>(StringComparer.Ordinal);
        foreach (var configPath in configPaths)
        {
            ModuleConfiguration config;
            try
            {
                config = LoadConfiguration(configPath, request.RepositoryRoot);
            }
            catch (Exception ex) when (ex is InvalidDataException or JsonException or IOException)
            {
                diagnostics.Add(new GenerationDiagnostic
                {
                    Severity = "error",
                    Code = "AXLUA002",
                    Message = $"Invalid binding configuration '{configPath}': {ex.Message}",
                    Header = configPath
                });
                continue;
            }

            var generationRequest = config.ToRequest(request);
            var result = BindingGenerator.Generate(generationRequest);
            results.Add(result);
            diagnostics.AddRange(result.Diagnostics);

            foreach (var bindingClass in result.Classes)
            {
                var luaName = $"{config.LuaNamespace}.{bindingClass.LuaClassName}";
                if (registeredClasses.TryGetValue(luaName, out var previous) &&
                    !string.Equals(previous.NativeName, bindingClass.QualifiedName, StringComparison.Ordinal))
                {
                    diagnostics.Add(new GenerationDiagnostic
                    {
                        Severity = "error",
                        Code = "AXLUA011",
                        Message = $"Lua class '{luaName}' is registered by both " +
                                  $"{previous.Module}:{previous.NativeName} and " +
                                  $"{config.Module}:{bindingClass.QualifiedName}."
                    });
                }
                else
                {
                    registeredClasses[luaName] = (config.Module, bindingClass.QualifiedName);
                }
            }
        }

        return new BatchGenerationResult
        {
            Results = results,
            Diagnostics = diagnostics
        };
    }

    private static void ValidateRequest(BatchGenerationRequest request)
    {
        if (string.IsNullOrWhiteSpace(request.RepositoryRoot))
            throw new ArgumentException("RepositoryRoot is required.", nameof(request));
        if (string.IsNullOrWhiteSpace(request.ConfigurationDirectory))
            throw new ArgumentException("ConfigurationDirectory is required.", nameof(request));
        if (string.IsNullOrWhiteSpace(request.OutputDirectory))
            throw new ArgumentException("OutputDirectory is required.", nameof(request));
        if (!Directory.Exists(request.ConfigurationDirectory))
            throw new DirectoryNotFoundException(request.ConfigurationDirectory);
    }

    private static IReadOnlyList<string> SelectConfigurations(string directory, string module)
    {
        var files = Directory.GetFiles(directory, "ax_*.json", SearchOption.TopDirectoryOnly)
            .OrderBy(x => x, StringComparer.OrdinalIgnoreCase)
            .ToArray();
        if (string.Equals(module, "all", StringComparison.OrdinalIgnoreCase))
            return files;

        var normalized = module.StartsWith("ax_", StringComparison.OrdinalIgnoreCase) ? module : $"ax_{module}";
        var selected = files.FirstOrDefault(path =>
            string.Equals(Path.GetFileNameWithoutExtension(path), normalized, StringComparison.OrdinalIgnoreCase));
        if (selected is null)
            throw new InvalidDataException($"Unknown binding module '{module}'. Available modules: " +
                                            string.Join(", ", files.Select(x => Path.GetFileNameWithoutExtension(x))));
        return new[] { selected };
    }

    private static ModuleConfiguration LoadConfiguration(string path, string repositoryRoot)
    {
        using var document = JsonDocument.Parse(File.ReadAllText(path));
        var root = document.RootElement;
        if (root.ValueKind != JsonValueKind.Object)
            throw new InvalidDataException("The configuration root must be an object.");
        foreach (var property in root.EnumerateObject())
        {
            if (!ConfigProperties.Contains(property.Name))
                throw new InvalidDataException($"Unknown property '{property.Name}'.");
        }

        var module = RequiredString(root, "module");
        var expected = Path.GetFileNameWithoutExtension(path);
        if (!string.Equals(module, expected, StringComparison.Ordinal))
            throw new InvalidDataException($"module '{module}' does not match file name '{expected}'.");
        if (!Regex.IsMatch(module, @"^ax_[a-z0-9_]+$"))
            throw new InvalidDataException($"module '{module}' is not a valid Axmol binding module name.");

        var headers = RequiredStrings(root, "headers").Select(x => ExpandPath(x, repositoryRoot)).ToArray();
        var nativeNamespaces = OptionalStringOrStrings(root, "cpp_namespace")
            .Select(x => x.Trim()).Where(x => x.Length > 0).ToArray();
        var targetNamespace = RequiredString(root, "target_namespace").Trim();
        var conditional = OptionalString(root, "macro_judgement");
        if (conditional is not null)
        {
            var match = Regex.Match(conditional, @"^\s*#if\s+(.+?)\s*$");
            conditional = match.Success ? match.Groups[1].Value : conditional;
            if (string.IsNullOrWhiteSpace(conditional))
                conditional = null;
        }

        var noConstructors = OptionalStrings(root, "abstract_classes")
            .Concat(OptionalStrings(root, "no_constructors"))
            .ToArray();
        var chunks = OptionalInt(root, "cpp_chunks") ?? 1;
        if (chunks < 1)
            throw new InvalidDataException("cpp_chunks must be at least 1.");

        return new ModuleConfiguration
        {
            Module = module,
            LuaNamespace = targetNamespace,
            LuaTypeNamespace = targetNamespace,
            Headers = headers,
            ClangFlags = OptionalStrings(root, "clang_flags"),
            NativeNamespaces = nativeNamespaces,
            ClassPatterns = OptionalStrings(root, "classes"),
            NoConstructorPatterns = noConstructors,
            NoParentPatterns = OptionalStrings(root, "classes_have_no_parents"),
            SkippedBasePatterns = OptionalStrings(root, "base_classes_to_skip"),
            SkipRules = ReadSkipRules(root),
            FieldRules = ReadFieldRules(root),
            RenameRules = ReadRenameRules(root),
            ClassRenames = ReadClassRenames(root),
            FastBindings = OptionalStrings(root, "fast_bindings"),
            ConditionalExpression = conditional,
            CppChunkCount = chunks
        };
    }

    private static void ValidateManualAdapters(BatchGenerationRequest request)
    {
        if (string.IsNullOrWhiteSpace(request.AdapterAllowlistPath))
            return;
        if (!File.Exists(request.AdapterAllowlistPath))
            throw new FileNotFoundException("Manual adapter allowlist is missing.", request.AdapterAllowlistPath);

        using var document = JsonDocument.Parse(File.ReadAllText(request.AdapterAllowlistPath));
        if (!document.RootElement.TryGetProperty("adapters", out var entries) ||
            entries.ValueKind != JsonValueKind.Array || entries.GetArrayLength() == 0)
            throw new InvalidDataException("Manual adapter allowlist must contain at least one adapter entry.");

        var listed = new HashSet<string>(StringComparer.Ordinal);
        foreach (var entry in entries.EnumerateArray())
        {
            var path = RequiredString(entry, "path").Replace('\\', '/');
            var reason = RequiredString(entry, "reason").Trim();
            if (reason.Length == 0)
                throw new InvalidDataException($"Manual adapter '{path}' has an empty reason.");
            if (!listed.Add(path))
                throw new InvalidDataException($"Manual adapter is listed more than once: {path}");
            if (!path.StartsWith("extensions/scripting/lua-bindings/adapters/", StringComparison.Ordinal) ||
                !path.EndsWith(".cpp", StringComparison.Ordinal))
                throw new InvalidDataException($"Manual adapter allowlist path is outside adapters/: {path}");
            if (!File.Exists(Path.Combine(request.RepositoryRoot, path.Replace('/', Path.DirectorySeparatorChar))))
                throw new FileNotFoundException($"Manual adapter allowlist points to a missing file: {path}");
        }

        var adapterRoot = Path.Combine(request.RepositoryRoot, "extensions", "scripting", "lua-bindings", "adapters");
        foreach (var actualPath in Directory.EnumerateFiles(adapterRoot, "*.cpp", SearchOption.AllDirectories))
        {
            var relative = Path.GetRelativePath(request.RepositoryRoot, actualPath).Replace('\\', '/');
            if (!listed.Contains(relative))
                throw new InvalidDataException(
                    $"Manual adapter is not allowlisted: {relative}. Add an explicit reason or remove the adapter.");
        }
    }

    private static string ExpandPath(string value, string repositoryRoot) =>
        Regex.Replace(value.Trim('"'), @"%\([^)]+\)s", repositoryRoot).Replace('\\', '/');

    private static string RequiredString(JsonElement root, string name)
    {
        if (!root.TryGetProperty(name, out var value) || value.ValueKind != JsonValueKind.String ||
            string.IsNullOrWhiteSpace(value.GetString()))
            throw new InvalidDataException($"'{name}' must be a non-empty string.");
        return value.GetString()!;
    }

    private static string? OptionalString(JsonElement root, string name)
    {
        if (!root.TryGetProperty(name, out var value) || value.ValueKind == JsonValueKind.Null)
            return null;
        if (value.ValueKind != JsonValueKind.String)
            throw new InvalidDataException($"'{name}' must be a string or null.");
        return value.GetString();
    }

    private static int? OptionalInt(JsonElement root, string name)
    {
        if (!root.TryGetProperty(name, out var value))
            return null;
        if (!value.TryGetInt32(out var result))
            throw new InvalidDataException($"'{name}' must be an integer.");
        return result;
    }

    private static IReadOnlyList<string> RequiredStrings(JsonElement root, string name)
    {
        if (!root.TryGetProperty(name, out var value) || value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException($"'{name}' must be an array of strings.");
        return ReadStrings(value, name, true);
    }

    private static JsonElement RequiredArray(JsonElement root, string name)
    {
        if (!root.TryGetProperty(name, out var value) || value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException($"'{name}' must be an array of strings.");
        return value;
    }

    private static void ValidateProperties(JsonElement value, string context, params string[] allowed)
    {
        if (value.ValueKind != JsonValueKind.Object)
            throw new InvalidDataException($"'{context}' entries must be objects.");
        var names = allowed.ToHashSet(StringComparer.Ordinal);
        foreach (var property in value.EnumerateObject())
        {
            if (!names.Contains(property.Name))
                throw new InvalidDataException($"Unknown property '{property.Name}' in {context} entry.");
        }
    }

    private static IReadOnlyList<string> OptionalStrings(JsonElement root, string name) =>
        root.TryGetProperty(name, out var value) ? ReadStrings(value, name, false) : Array.Empty<string>();

    private static IReadOnlyList<string> OptionalStringOrStrings(JsonElement root, string name)
    {
        if (!root.TryGetProperty(name, out var value) || value.ValueKind == JsonValueKind.Null)
            return Array.Empty<string>();
        if (value.ValueKind == JsonValueKind.String)
            return new[] { value.GetString()! };
        return ReadStrings(value, name, false);
    }

    private static IReadOnlyList<string> ReadStrings(JsonElement value, string name, bool required)
    {
        if (value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException($"'{name}' must be an array of strings.");
        var result = value.EnumerateArray().Select(x =>
        {
            if (x.ValueKind != JsonValueKind.String)
                throw new InvalidDataException($"'{name}' must contain only strings.");
            return x.GetString()!;
        }).Where(x => x.Length > 0).ToArray();
        if (required && result.Length == 0)
            throw new InvalidDataException($"'{name}' must not be empty.");
        return result;
    }

    private static IReadOnlyList<BindingSkipRule> ReadSkipRules(JsonElement root)
    {
        if (!root.TryGetProperty("skip", out var value))
            return Array.Empty<BindingSkipRule>();
        if (value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException("'skip' must be an array.");
        return value.EnumerateArray().Select(entry =>
        {
            ValidateProperties(entry, "skip", "class", "methods");
            var methods = RequiredArray(entry, "methods");
            return new BindingSkipRule
            {
                ClassPattern = RequiredString(entry, "class"),
                MethodPatterns = ReadStrings(methods, "methods", false)
            };
        }).Where(x => x.MethodPatterns.Count > 0).ToArray();
    }

    private static IReadOnlyList<BindingFieldRule> ReadFieldRules(JsonElement root)
    {
        if (!root.TryGetProperty("fields", out var value))
            return Array.Empty<BindingFieldRule>();
        if (value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException("'fields' must be an array.");
        return value.EnumerateArray().Select(entry =>
        {
            ValidateProperties(entry, "fields", "class", "names");
            var names = RequiredArray(entry, "names");
            return new BindingFieldRule
            {
                ClassPattern = RequiredString(entry, "class"),
                FieldPatterns = ReadStrings(names, "names", false)
            };
        }).Where(x => x.FieldPatterns.Count > 0).ToArray();
    }

    private static IReadOnlyList<BindingRenameRule> ReadRenameRules(JsonElement root)
    {
        if (!root.TryGetProperty("rename_functions", out var value))
            return Array.Empty<BindingRenameRule>();
        if (value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException("'rename_functions' must be an array.");
        return value.EnumerateArray().Select(entry =>
        {
            ValidateProperties(entry, "rename_functions", "class", "method", "luaName");
            return new BindingRenameRule
            {
                ClassPattern = RequiredString(entry, "class"),
                MethodPattern = RequiredString(entry, "method"),
                LuaName = RequiredString(entry, "luaName")
            };
        }).ToArray();
    }

    private static IReadOnlyList<BindingClassRename> ReadClassRenames(JsonElement root)
    {
        if (!root.TryGetProperty("rename_classes", out var value))
            return Array.Empty<BindingClassRename>();
        if (value.ValueKind != JsonValueKind.Array)
            throw new InvalidDataException("'rename_classes' must be an array.");
        return value.EnumerateArray().Select(entry =>
        {
            ValidateProperties(entry, "rename_classes", "native", "luaName");
            return new BindingClassRename
            {
                NativeName = RequiredString(entry, "native"),
                LuaName = RequiredString(entry, "luaName")
            };
        }).ToArray();
    }

    private sealed class ModuleConfiguration
    {
        public required string Module { get; init; }
        public required string LuaNamespace { get; init; }
        public required string LuaTypeNamespace { get; init; }
        public required IReadOnlyList<string> Headers { get; init; }
        public required IReadOnlyList<string> ClangFlags { get; init; }
        public required IReadOnlyList<string> NativeNamespaces { get; init; }
        public required IReadOnlyList<string> ClassPatterns { get; init; }
        public required IReadOnlyList<string> NoConstructorPatterns { get; init; }
        public required IReadOnlyList<string> NoParentPatterns { get; init; }
        public required IReadOnlyList<string> SkippedBasePatterns { get; init; }
        public required IReadOnlyList<BindingSkipRule> SkipRules { get; init; }
        public required IReadOnlyList<BindingFieldRule> FieldRules { get; init; }
        public required IReadOnlyList<BindingRenameRule> RenameRules { get; init; }
        public required IReadOnlyList<BindingClassRename> ClassRenames { get; init; }
        public required IReadOnlyList<string> FastBindings { get; init; }
        public string? ConditionalExpression { get; init; }
        public int CppChunkCount { get; init; }

        public GenerationRequest ToRequest(BatchGenerationRequest batch)
        {
            var clangArguments = batch.ClangArguments.Concat(ClangFlags).ToArray();
            return new GenerationRequest
            {
                RepositoryRoot = batch.RepositoryRoot,
                OutputDirectory = batch.OutputDirectory,
                Module = Module[3..],
                RegistrationName = Module,
                Headers = Headers,
                ClangArguments = clangArguments,
                NativeNamespaces = NativeNamespaces,
                ClassPatterns = ClassPatterns,
                NoConstructorPatterns = NoConstructorPatterns,
                NoParentPatterns = NoParentPatterns,
                SkippedBasePatterns = SkippedBasePatterns,
                SkipRules = SkipRules,
                FieldRules = FieldRules,
                RenameRules = RenameRules,
                ClassRenames = ClassRenames,
                FastBindings = FastBindings,
                LuaNamespace = LuaNamespace,
                LuaTypeNamespace = LuaTypeNamespace,
                ConditionalExpression = ConditionalExpression,
                CppChunkCount = CppChunkCount,
                EmitCpp = !batch.Verify,
                EmitManifest = !batch.Verify
            };
        }
    }
}
