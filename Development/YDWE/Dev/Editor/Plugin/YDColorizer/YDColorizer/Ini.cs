using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace YDColorizer
{
    public class Ini
    {
        private string _contents;

        public Ini(string contents_)
        {
            _contents = contents_;
        }

        public string[] SectionNames
        {
            get
            {
                // Using regular expression to get all section names.
                string regexPattern = @"\[(?<SectionName>\w*)\]";
                Regex r = new Regex(regexPattern); // Match "[anywords]"
                MatchCollection matches = r.Matches(_contents);
                // Writing all section names to a string array.
                string[] results = new string[matches.Count];
                for (int i = 0; i < matches.Count; i++)
                {
                    results[i] = matches[i].Result("${SectionName}");
                }
                return results;
            }
        }

        public string GetSectionString(string sectionName)
        {
            string regexPattern = @"(\[" + sectionName + @"\]" + @"(?<SectionString>.*)\[)";
            Regex r = new Regex(regexPattern, RegexOptions.Singleline);
            if (r.IsMatch(_contents))
            {
                return r.Match(_contents).Result("${SectionString}");
            }
            return string.Empty;
        }
　　
        public string GetKeyString(string sectionName, string keyName)
        {
            string sectionString = this.GetSectionString(sectionName);
            string regexPattern = @"(" + keyName + @"=(?<value>.*)\r\n)";
            Regex r = new Regex(regexPattern);
            if (r.IsMatch(sectionString))
            {
                return r.Match(sectionString).Result("${value}");
            }

            return string.Empty;
        }

        public Section GetSection(string sectionName)
        {
            string sectionString = this.GetSectionString(sectionName);

            return new Section(sectionString);
        }

        public class Section
        {
            private string _contents;

            public Section(string contents_)
            {
                _contents = contents_;
            }

            public string GetKeyString(string keyName)
            {
                string regexPattern = @"(" + keyName + @"=(?<value>.*)\r\n)";
                Regex r = new Regex(regexPattern);
                if (r.IsMatch(_contents))
                {
                    return r.Match(_contents).Result("${value}");
                }

                return string.Empty;
            }
        }
    }
}
