#ifndef INCLUDE_WARCRAFTIIIKEYBOARDCODE_H
#define INCLUDE_WARCRAFTIIIKEYBOARDCODE_H

#define WARK_SHIFT          0x0
#define WARK_CTRL           0x1
#define WARK_ALT            0x2
#define WARK_SPACE          0x20

#define WARK_0              0x30
#define WARK_1              0x31
#define WARK_2              0x32
#define WARK_3              0x33
#define WARK_4              0x34
#define WARK_5              0x35
#define WARK_6              0x36
#define WARK_7              0x37
#define WARK_8              0x38
#define WARK_9              0x39

#define WARK_A              0x41
#define WARK_B              0x42
#define WARK_C              0x43
#define WARK_D              0x44
#define WARK_E              0x45
#define WARK_F              0x46
#define WARK_G              0x47
#define WARK_H              0x48
#define WARK_I              0x49
#define WARK_J              0x4A
#define WARK_K              0x4B
#define WARK_L              0x4C
#define WARK_M              0x4D
#define WARK_N              0x4E
#define WARK_O              0x4F
#define WARK_P              0x50
#define WARK_Q              0x51
#define WARK_R              0x52
#define WARK_S              0x53
#define WARK_T              0x54
#define WARK_U              0x55
#define WARK_V              0x56
#define WARK_W              0x57
#define WARK_X              0x58
#define WARK_Y              0x59
#define WARK_Z              0x5A

#define WARK_TILDE          0x100
#define WARK_NUMPAD0        0x101
#define WARK_NUMPAD1        0x102
#define WARK_NUMPAD2        0x103
#define WARK_NUMPAD3        0x104
#define WARK_NUMPAD4        0x105
#define WARK_NUMPAD5        0x106
#define WARK_NUMPAD6        0x107
#define WARK_NUMPAD7        0x108
#define WARK_NUMPAD8        0x109
#define WARK_NUMPAD9        0x10A
#define WARK_ADD            0x10B
#define WARK_SUBTRACT       0x10C
#define WARK_MULTIPLY       0x10D
#define WARK_DIVIDE         0x10E
#define WARK_DECIMAL        0x10F

#define WARK_OEM_PLUS       0x110   // '=(+)'
#define WARK_OEM_MINUS      0x111   // '-(_)'
#define WARK_OEM_OBRACKE    0x112   // '[({)'
#define WARK_OEM_CBRACKE    0x113   // '](})'
#define WARK_OEM_BACKSLASH  0x114   // '\(|)'
#define WARK_OEM_SEMICOLON  0x115   // ';(:)'
#define WARK_OEM_SQUOTMARKS 0x116   // ''(")'
#define WARK_OEM_COMMA      0x117   // ',(<)'
#define WARK_OEM_PERIOD     0x118   // '.(>)'
#define WARK_OEM_SLASH      0x119   // '/(?)'

#define WARK_ESC            0x200
#define WARK_ENTER          0x201
#define WARK_BACKSPACE      0x202
#define WARK_TAB            0x203
#define WARK_RIGHT          0x204
#define WARK_UP             0x205
#define WARK_LEFT           0x206
#define WARK_DOWN           0x207
#define WARK_INSERT         0x208
#define WARK_DELETE         0x209
#define WARK_HOME           0x20A
#define WARK_END            0x20B
#define WARK_PAGEUP         0x20C
#define WARK_PAGEDOWN       0x20D
#define WARK_CAPSLOCK       0x20E
#define WARK_NUMLOCK        0x20F
#define WARK_SCROLLLOCK     0x210
#define WARK_PAUSE          0x211
#define WARK_PRINTSCREEN    0x212

#define WARK_F1             0x300
#define WARK_F2             0x301
#define WARK_F3             0x302
#define WARK_F4             0x303
#define WARK_F5             0x304
#define WARK_F6             0x305
#define WARK_F7             0x306
#define WARK_F8             0x307
#define WARK_F9             0x308
#define WARK_F10            0x309
#define WARK_F11            0x30A
#define WARK_F12            0x30B


#define SET_KEYBOARD_ALL() \
	SET_KEYBOARD(SHIFT);\
	SET_KEYBOARD(CTRL);\
	SET_KEYBOARD(ALT);\
	SET_KEYBOARD(SPACE);\
	SET_KEYBOARD(0);\
	SET_KEYBOARD(1);\
	SET_KEYBOARD(2);\
	SET_KEYBOARD(3);\
	SET_KEYBOARD(4);\
	SET_KEYBOARD(5);\
	SET_KEYBOARD(6);\
	SET_KEYBOARD(7);\
	SET_KEYBOARD(8);\
	SET_KEYBOARD(9);\
	SET_KEYBOARD(A);\
	SET_KEYBOARD(B);\
	SET_KEYBOARD(C);\
	SET_KEYBOARD(D);\
	SET_KEYBOARD(E);\
	SET_KEYBOARD(F);\
	SET_KEYBOARD(G);\
	SET_KEYBOARD(H);\
	SET_KEYBOARD(I);\
	SET_KEYBOARD(J);\
	SET_KEYBOARD(K);\
	SET_KEYBOARD(L);\
	SET_KEYBOARD(M);\
	SET_KEYBOARD(N);\
	SET_KEYBOARD(O);\
	SET_KEYBOARD(P);\
	SET_KEYBOARD(Q);\
	SET_KEYBOARD(R);\
	SET_KEYBOARD(S);\
	SET_KEYBOARD(T);\
	SET_KEYBOARD(U);\
	SET_KEYBOARD(V);\
	SET_KEYBOARD(W);\
	SET_KEYBOARD(X);\
	SET_KEYBOARD(Y);\
	SET_KEYBOARD(Z);\
	SET_KEYBOARD(TILDE);\
	SET_KEYBOARD(NUMPAD0);\
	SET_KEYBOARD(NUMPAD1);\
	SET_KEYBOARD(NUMPAD2);\
	SET_KEYBOARD(NUMPAD3);\
	SET_KEYBOARD(NUMPAD4);\
	SET_KEYBOARD(NUMPAD5);\
	SET_KEYBOARD(NUMPAD6);\
	SET_KEYBOARD(NUMPAD7);\
	SET_KEYBOARD(NUMPAD8);\
	SET_KEYBOARD(NUMPAD9);\
	SET_KEYBOARD(ADD);\
	SET_KEYBOARD(SUBTRACT);\
	SET_KEYBOARD(MULTIPLY);\
	SET_KEYBOARD(DIVIDE);\
	SET_KEYBOARD(DECIMAL);\
	SET_KEYBOARD(OEM_PLUS);\
	SET_KEYBOARD(OEM_MINUS);\
	SET_KEYBOARD(OEM_OBRACKE);\
	SET_KEYBOARD(OEM_CBRACKE);\
	SET_KEYBOARD(OEM_BACKSLASH);\
	SET_KEYBOARD(OEM_SEMICOLON);\
	SET_KEYBOARD(OEM_SQUOTMARKS);\
	SET_KEYBOARD(OEM_COMMA);\
	SET_KEYBOARD(OEM_PERIOD);\
	SET_KEYBOARD(OEM_SLASH);\
	SET_KEYBOARD(ESC);\
	SET_KEYBOARD(ENTER);\
	SET_KEYBOARD(BACKSPACE);\
	SET_KEYBOARD(TAB);\
	SET_KEYBOARD(RIGHT);\
	SET_KEYBOARD(UP);\
	SET_KEYBOARD(LEFT);\
	SET_KEYBOARD(DOWN);\
	SET_KEYBOARD(INSERT);\
	SET_KEYBOARD(DELETE);\
	SET_KEYBOARD(HOME);\
	SET_KEYBOARD(END);\
	SET_KEYBOARD(PAGEUP);\
	SET_KEYBOARD(PAGEDOWN);\
	SET_KEYBOARD(CAPSLOCK);\
	SET_KEYBOARD(NUMLOCK);\
	SET_KEYBOARD(SCROLLLOCK);\
	SET_KEYBOARD(PAUSE);\
	SET_KEYBOARD(PRINTSCREEN);\
	SET_KEYBOARD(F1);\
	SET_KEYBOARD(F2);\
	SET_KEYBOARD(F3);\
	SET_KEYBOARD(F4);\
	SET_KEYBOARD(F5);\
	SET_KEYBOARD(F6);\
	SET_KEYBOARD(F7);\
	SET_KEYBOARD(F8);\
	SET_KEYBOARD(F9);\
	SET_KEYBOARD(F10);\
	SET_KEYBOARD(F11);\
	SET_KEYBOARD(F12);

#endif
