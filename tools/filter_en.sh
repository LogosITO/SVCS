#!/usr/bin/awk -f

/@russian/ {
    in_russian=1
    next
}

/\*\// {
    in_russian=0
}

/@english/ {
    next
}

!in_russian {
    print
}