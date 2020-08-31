#!/usr/bin/awk

/echo command/ { exact = 1; print; next; }
/pwd command/ { exact = 0; print; print "(line(s) we don't care)"; next; }
/outside command/ { exact = 1; print; next; }
/for, seq, if-elif-else/ { exact = 1; print; next; }
/echo, cat, rm command/ { exact = 1; print; next; }
/shellfunction/ { exact = 1; print; next; }
/ls command/ { exact = 0; print; print "(line(s) we don't care)"; next; }
/shell end/ { exact = 1; print; next; }
{ if (exact == 1) { print } }
