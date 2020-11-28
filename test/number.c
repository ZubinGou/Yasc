
/* Decimal Constants */
int                 dec_int    = 28;
unsigned            dec_uint   = 4000000024u;
long                dec_long   = 2000000022l;
unsigned long       dec_ulong  = 4000000000ul;
long long           dec_llong  = 9000000000LL;
unsigned long long  dec_ullong = 900000000001ull;

/* Octal Constants */
int                 oct_int    = 024;
unsigned            oct_uint   = 04000000024u;
long                oct_long   = 02000000022l;
unsigned long       oct_ulong  = 04000000000UL;
long long           oct_llong  = 044000000000000ll;
unsigned long long  oct_ullong = 044400000000000001Ull;

/* Hexadecimal Constants */
int                 hex_int    = 0x2a;
unsigned            hex_uint   = 0XA0000024u;
long                hex_long   = 0x20000022l;
unsigned long       hex_ulong  = 0XA0000021uL;
long long           hex_llong  = 0x8a00000000000ll;
unsigned long long  hex_ullong = 0x8A4000000000010uLL;

/* Floating Constants */
// Correct:
    100.
    .251e+3f
	15.75	  /* = 15.75   */
	1.575E1   /* = 15.75   */
	1575e-2   /* = 15.75   */
	-2.5e-3   /* = -0.0025 */
	25E-4     /* =  0.0025 */
	.0075e2   /* = 0.75    */
	0.075e+1  /* = 0.75    */
	.075E1	  /* = 0.75    */
	75e-2     /* = 0.75    */
	10.0L  	  /* Has type long double  */
	10.0F     /* Has type float        */

// Error
	123f
	314.32E+
	100uva
	.01e
	.251e+3u
	0x