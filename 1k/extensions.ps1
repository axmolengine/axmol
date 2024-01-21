# The System.Version compare not relex: [Version]'1.0' -eq [Version]'1.0.0' == false
# So provide a spec VersionEx make [VersionEx]'1.0' -eq [VersionEx]'1.0.0' == true available
if (-not ([System.Management.Automation.PSTypeName]'System.VersionEx').Type) {

    Add-Type -TypeDefinition @"

namespace System
{
    public sealed class VersionEx : ICloneable, IComparable
    {
        int _Major = 0;
        int _Minor = 0;
        int _Build = 0;
        int _Revision = 0;

        public int Minor { get { return _Major;  } }

        public int Major { get { return _Minor; } }

        public int Build { get { return _Build; } }

        public int Revision { get { return _Revision; } }

        int DefaultFormatFieldCount { get { return (_Build > 0 || _Revision > 0) ? (_Revision > 0 ? 4 : 3) : 2; } }
 
        public VersionEx() { }

        public VersionEx(string version)
        {
            var v = Parse(version);
            _Major = v.Major;
            _Minor = v.Minor;
            _Build = v.Build;
            _Revision = v.Revision;
        }

        public VersionEx(System.Version version) { 
            _Major = version.Major;
            _Minor = version.Minor;
            _Build = Math.Max(version.Build, 0);
            _Revision = Math.Max(version.Revision, 0);
        }

        public VersionEx(int major, int minor, int build, int revision)
        {
            _Major = major;
            _Minor = minor;
            _Build = build;
            _Revision = revision;
        }

        public static VersionEx Parse(string input)
        {
            var versionNums = input.Split('.');
            int major = 0;
            int minor = 0;
            int build = 0;
            int revision = 0;
            for (int i = 0; i < versionNums.Length; ++i)
            {
                switch (i)
                {
                    case 0:
                        major = int.Parse(versionNums[i]);
                        break;
                    case 1:
                        minor = int.Parse(versionNums[i]);
                        break;
                    case 2:
                        build = int.Parse(versionNums[i]);
                        break;
                    case 3:
                        revision = int.Parse(versionNums[i]);
                        break;
                }
            }
            return new VersionEx(major, minor, build, revision);
        }

        public static bool TryParse(string input, out VersionEx result)
        {
            try
            {
                result = VersionEx.Parse(input);
                return true;
            }
            catch (Exception)
            {
                result = null;
                return false;
            }
        }

        public object Clone()
        {
            return new VersionEx(Major, Minor, Build, Revision);
        }

        public int CompareTo(object obj)
        {
            if (obj is VersionEx)
            {
                return CompareTo((VersionEx)obj);
            }
            else if (obj is Version)
            {
                var rhs = (Version)obj;
                return _Major != rhs.Major ? (_Major > rhs.Major ? 1 : -1) :
                _Minor != rhs.Minor ? (_Minor > rhs.Minor ? 1 : -1) :
                _Build != rhs.Build ? (_Build > rhs.Build ? 1 : -1) :
                _Revision != rhs.Revision ? (_Revision > rhs.Revision ? 1 : -1) :
                0;
            }
            else return 1;
        }

        public int CompareTo(VersionEx obj)
        {
            return
                 ReferenceEquals(obj, this) ? 0 :
                 ReferenceEquals(obj, null) ? 1 :
                 _Major != obj._Major ? (_Major > obj._Major ? 1 : -1) :
                 _Minor != obj._Minor ? (_Minor > obj._Minor ? 1 : -1) :
                 _Build != obj._Build ? (_Build > obj._Build ? 1 : -1) :
                 _Revision != obj._Revision ? (_Revision > obj._Revision ? 1 : -1) :
                 0;
        }

        public bool Equals(VersionEx obj)
        {
            return CompareTo(obj) == 0;
        }

        public override bool Equals(object obj)
        {
            return CompareTo(obj) == 0;
        }

        public override string ToString()
        {
            return ToString(DefaultFormatFieldCount);
        }

        public string ToString(int fieldCount)
        {
            switch (fieldCount)
            {
                case 2:
                    return string.Format("{0}.{1}", _Major, _Minor);
                case 3:
                    return string.Format("{0}.{1}.{2}", _Major, _Minor, _Build);
                case 4:
                    return string.Format("{0}.{1}.{2}.{3}", _Major, _Minor, _Build, _Revision);
                default:
                    return "0.0.0.0";
            }
        }

        public override int GetHashCode()
        {
            // Let's assume that most version numbers will be pretty small and just
            // OR some lower order bits together.

            int accumulator = 0;

            accumulator |= (_Major & 0x0000000F) << 28;
            accumulator |= (_Minor & 0x000000FF) << 20;
            accumulator |= (_Build & 0x000000FF) << 12;
            accumulator |= (_Revision & 0x00000FFF);

            return accumulator;
        }

        public static bool operator ==(VersionEx v1, VersionEx v2)
        {
            return v1.Equals(v2);
        }

        public static bool operator !=(VersionEx v1, VersionEx v2)
        {
            return !v1.Equals(v2);
        }

        public static bool operator <(VersionEx v1, VersionEx v2)
        {
            return v1.CompareTo(v2) < 0;
        }

        public static bool operator >(VersionEx v1, VersionEx v2)
        {
            return v1.CompareTo(v2) > 0;
        }

        public static bool operator <=(VersionEx v1, VersionEx v2)
        {
            return v1.CompareTo(v2) <= 0;
        }

        public static bool operator >=(VersionEx v1, VersionEx v2)
        {
            return v1.CompareTo(v2) >= 0;
        }
    }

    public static class ExtensionMethods
    {
        public static string TrimLast(this Management.Automation.PSObject thiz, string separator)
        {
            var str = thiz.BaseObject as string;
            var index = str.LastIndexOf(separator);
            if (index != -1)
                return str.Substring(0, index);
            return str;
        }
    }
}
"@

$TrimLastMethod = [ExtensionMethods].GetMethod('TrimLast')
Update-TypeData -TypeName System.String -MemberName TrimLast -MemberType CodeMethod -Value $TrimLastMethod
}
