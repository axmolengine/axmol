using System;
using System.Collections.Generic;
using System.Text.Json.Serialization;

namespace Axmol.LuaBindings;

public sealed class GenerationRequest
{
    public required string RepositoryRoot { get; init; }
    public required string OutputDirectory { get; init; }
    public required string Module { get; init; }
    public string RegistrationName { get; init; } = "";
    public required IReadOnlyList<string> Headers { get; init; }
    public IReadOnlyList<string> ClangArguments { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> NativeNamespaces { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> ClassPatterns { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> NoConstructorPatterns { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> NoParentPatterns { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> SkippedBasePatterns { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> SkipPatterns { get; init; } = Array.Empty<string>();
    public IReadOnlyList<BindingSkipRule> SkipRules { get; init; } = Array.Empty<BindingSkipRule>();
    public IReadOnlyList<BindingFieldRule> FieldRules { get; init; } = Array.Empty<BindingFieldRule>();
    public IReadOnlyList<BindingRenameRule> RenameRules { get; init; } = Array.Empty<BindingRenameRule>();
    public IReadOnlyList<BindingClassRename> ClassRenames { get; init; } = Array.Empty<BindingClassRename>();
    public string LuaNamespace { get; init; } = "ax";
    public string LuaTypeNamespace { get; init; } = "ax";
    public string? ConditionalExpression { get; init; }
    public int CppChunkCount { get; init; } = 1;
    public bool EmitCpp { get; init; } = true;
    public bool EmitManifest { get; init; } = true;
}

public sealed class GenerationResult
{
    public required string Module { get; init; }
    public required string ManifestPath { get; init; }
    public required IReadOnlyList<string> GeneratedFiles { get; init; }
    public required IReadOnlyList<GenerationDiagnostic> Diagnostics { get; init; }
    public required IReadOnlyList<BindingClass> Classes { get; init; }
    public required IReadOnlyList<BindingEnum> Enums { get; init; }
}

public sealed class BindingManifest
{
    public int FormatVersion { get; init; } = 1;
    public required string Module { get; init; }
    public required string RegistrationName { get; init; }
    public required string LuaNamespace { get; init; }
    public required string LuaTypeNamespace { get; init; }
    public string? ConditionalExpression { get; init; }
    public IReadOnlyList<BindingClass> Classes { get; init; } = Array.Empty<BindingClass>();
    public IReadOnlyList<BindingEnum> Enums { get; init; } = Array.Empty<BindingEnum>();
}

public sealed class GenerationDiagnostic
{
    public required string Severity { get; init; }
    public required string Code { get; init; }
    public required string Message { get; init; }
    public string? Header { get; init; }
}

public sealed class BindingClass
{
    public required string NativeName { get; init; }
    public required string QualifiedName { get; init; }
    public required string LuaName { get; init; }
    public required string LuaClassName { get; init; }
    public bool IsAbstract { get; init; }
    public IReadOnlyList<BindingConstructor> Constructors { get; init; } = Array.Empty<BindingConstructor>();
    public IReadOnlyList<string> Bases { get; init; } = Array.Empty<string>();
    public IReadOnlyList<BindingMethod> Methods { get; init; } = Array.Empty<BindingMethod>();
    public IReadOnlyList<BindingField> Fields { get; init; } = Array.Empty<BindingField>();
}

public sealed class BindingConstructor
{
    public IReadOnlyList<string> ParameterTypes { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string?> DefaultValues { get; init; } = Array.Empty<string?>();
}

public sealed class BindingMethod
{
    public required string NativeName { get; init; }
    public required string LuaName { get; init; }
    public required string DeclaringType { get; init; }
    public required string ReturnType { get; init; }
    public required IReadOnlyList<string> ParameterTypes { get; init; }
    public IReadOnlyList<string?> DefaultValues { get; init; } = Array.Empty<string?>();
    public bool IsStatic { get; init; }
    public bool IsConst { get; init; }
    public bool IsVariadic { get; init; }
}

public sealed class BindingField
{
    public required string NativeName { get; init; }
    public required string LuaName { get; init; }
    public required string FieldType { get; init; }
}

public sealed class BindingSkipRule
{
    public required string ClassPattern { get; init; }
    public required IReadOnlyList<string> MethodPatterns { get; init; }
}

/// <summary>
/// Opt-in public data-member export.  Fields are intentionally never exported
/// by a wildcard: data members are ABI and ownership-sensitive, so each
/// module declares the small stable surface it needs.
/// </summary>
public sealed class BindingFieldRule
{
    public required string ClassPattern { get; init; }
    public required IReadOnlyList<string> FieldPatterns { get; init; }
}

public sealed class BindingRenameRule
{
    public required string ClassPattern { get; init; }
    public required string MethodPattern { get; init; }
    public required string LuaName { get; init; }
}

public sealed class BindingClassRename
{
    public required string NativeName { get; init; }
    public required string LuaName { get; init; }
}

public sealed class BindingEnum
{
    public required string NativeName { get; init; }
    public required string QualifiedName { get; init; }
    public string? OwnerQualifiedName { get; init; }
    public IReadOnlyList<BindingEnumValue> Values { get; init; } = Array.Empty<BindingEnumValue>();
}

public sealed class BindingEnumValue
{
    public required string Name { get; init; }
    public long SignedValue { get; init; }
    public ulong UnsignedValue { get; init; }
    public bool IsUnsigned { get; init; }
}
