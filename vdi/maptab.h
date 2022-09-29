#define atari_to_utf16 __hidden_atari_to_utf16
#define atarifont_to_utf16 __hidden_atarifont_to_utf16
#define utf16_to_atari __hidden_utf16_to_atari

VISIBILITY("hidden") extern unsigned short const atari_to_utf16[256];
VISIBILITY("hidden") extern unsigned short const atarifont_to_utf16[256];
VISIBILITY("hidden") extern const unsigned char (*const utf16_to_atari[256])[256];
