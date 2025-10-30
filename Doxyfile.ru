PROJECT_NAME           = "SVCS - Русская документация"
OUTPUT_DIRECTORY       = docs/ru
INPUT                  = .
RECURSIVE              = YES
EXCLUDE                = build/ \
                         third_party/ \
                         CMakeFiles/
EXCLUDE_PATTERNS       = *.o \
                         *.a \
                         *.so \
                         *.exe \
                         *.bin
FILE_PATTERNS          = *.hxx \
                         *.cxx \
                         *.h \
                         *.cpp \
                         *.md
INPUT_FILTER           = "sed '/@english/,/@russian/d; s/@russian//g; s/@ru\\(brief\\|details\\)/@\\1/g'"
ALIASES                =
OUTPUT_LANGUAGE        = Russian
GENERATE_HTML          = YES
HTML_OUTPUT            = html
SEARCHENGINE           = YES