# Translator-C-

WORK IN PROGRESS

Basic translator using demo.wb as dictionary. 

Takes input through stdin, replaces every word with the corresponding word in another language using demo.wb dictionary.
Gives the output in stdout.

TO DOs:
1. Debug programm on Linux (find out why it does not print any words)
2. Replace '~' with EOF for end of input --- done
3. Implement de- and re-capitalization of words, so that words containing capital letters can be found in dictionary.
4. Re-classify numbers as separators (according to example given) --- done
5. Check malloc() and realloc()-s and make sure free() is present where needed
6. Check that string builders terminate string with '\0'
7. Test buffing 'currentWord' when input has long words
8. Adjust findWord(), so that it expects ':' surrounded by spaces in dictionary 
