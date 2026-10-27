# 16x2 lcd display stuff

Open this up in your favorite text editor that supports **monospace** fonts.

To write things, press insert to enter insert mode and modify the spaces inside.


### template

Empty
```
+################+
#                #
#                #
+################+
```

Code and physical layout guide
```
 [c0 c1 c2 c3 c...]
 +################+
 #                # [r0
 #AaBBBBBbCCCCcDDd#  r1]
 +################+
btnA btnB btnC btnD
```



### main screen
```
+################+
#mpe32board      #
#o-  o+  mcro cfg#
+################+
```