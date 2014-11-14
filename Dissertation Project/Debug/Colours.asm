; Listing generated by Microsoft (R) Optimizing Compiler Version 17.00.61030.0 

	TITLE	C:\Users\Simon\Documents\Visual Studio 2012\Projects\Dissertation Project\Dissertation Project\Colours.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB MSVCRTD
INCLUDELIB OLDNAMES

PUBLIC	?Min@@YAMMMM@Z					; Min
PUBLIC	?Max@@YAMMMM@Z					; Max
PUBLIC	?RGBToHSL@@YAXHHHAAH00@Z			; RGBToHSL
PUBLIC	?HSLToRGB@@YAXHHHAAH00@Z			; HSLToRGB
PUBLIC	?fHSLToRGB@@YAXMMMAAM00@Z			; fHSLToRGB
PUBLIC	?AdvanceHue@@YAXHAAM00@Z			; AdvanceHue
PUBLIC	__real@00000000
PUBLIC	__real@3f000000
PUBLIC	__real@3f800000
PUBLIC	__real@4000000000000000
PUBLIC	__real@4018000000000000
PUBLIC	__real@42480000
PUBLIC	__real@42700000
PUBLIC	__real@42c80000
PUBLIC	__real@42f00000
PUBLIC	__real@43700000
PUBLIC	__real@437f0000
PUBLIC	__real@43b40000
EXTRN	@_RTC_CheckStackVars@8:PROC
EXTRN	@__security_check_cookie@4:PROC
EXTRN	__RTC_CheckEsp:PROC
EXTRN	__RTC_InitBase:PROC
EXTRN	__RTC_Shutdown:PROC
EXTRN	___security_cookie:DWORD
EXTRN	__fltused:DWORD
;	COMDAT __real@43b40000
CONST	SEGMENT
__real@43b40000 DD 043b40000r			; 360
CONST	ENDS
;	COMDAT __real@437f0000
CONST	SEGMENT
__real@437f0000 DD 0437f0000r			; 255
CONST	ENDS
;	COMDAT __real@43700000
CONST	SEGMENT
__real@43700000 DD 043700000r			; 240
CONST	ENDS
;	COMDAT __real@42f00000
CONST	SEGMENT
__real@42f00000 DD 042f00000r			; 120
CONST	ENDS
;	COMDAT __real@42c80000
CONST	SEGMENT
__real@42c80000 DD 042c80000r			; 100
CONST	ENDS
;	COMDAT __real@42700000
CONST	SEGMENT
__real@42700000 DD 042700000r			; 60
CONST	ENDS
;	COMDAT __real@42480000
CONST	SEGMENT
__real@42480000 DD 042480000r			; 50
CONST	ENDS
;	COMDAT __real@4018000000000000
CONST	SEGMENT
__real@4018000000000000 DQ 04018000000000000r	; 6
CONST	ENDS
;	COMDAT __real@4000000000000000
CONST	SEGMENT
__real@4000000000000000 DQ 04000000000000000r	; 2
CONST	ENDS
;	COMDAT __real@3f800000
CONST	SEGMENT
__real@3f800000 DD 03f800000r			; 1
CONST	ENDS
;	COMDAT __real@3f000000
CONST	SEGMENT
__real@3f000000 DD 03f000000r			; 0.5
CONST	ENDS
;	COMDAT __real@00000000
CONST	SEGMENT
__real@00000000 DD 000000000r			; 0
CONST	ENDS
;	COMDAT rtc$TMZ
rtc$TMZ	SEGMENT
__RTC_Shutdown.rtc$TMZ DD FLAT:__RTC_Shutdown
rtc$TMZ	ENDS
;	COMDAT rtc$IMZ
rtc$IMZ	SEGMENT
__RTC_InitBase.rtc$IMZ DD FLAT:__RTC_InitBase
rtc$IMZ	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\visual studio 2012\projects\dissertation project\dissertation project\colours.cpp
;	COMDAT ?AdvanceHue@@YAXHAAM00@Z
_TEXT	SEGMENT
_newB$ = -72						; size = 4
_newG$ = -60						; size = 4
_newR$ = -48						; size = 4
_L$ = -36						; size = 4
_S$ = -24						; size = 4
_H$ = -12						; size = 4
__$ArrayPad$ = -4					; size = 4
_amount$ = 8						; size = 4
_R$ = 12						; size = 4
_G$ = 16						; size = 4
_B$ = 20						; size = 4
?AdvanceHue@@YAXHAAM00@Z PROC				; AdvanceHue, COMDAT

; 8    : {

	push	ebp
	mov	ebp, esp
	sub	esp, 268				; 0000010cH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-268]
	mov	ecx, 67					; 00000043H
	mov	eax, -858993460				; ccccccccH
	rep stosd
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	mov	DWORD PTR __$ArrayPad$[ebp], eax

; 9    : 	int H, S, L;
; 10   : 	RGBToHSL( int(R*255.0f), int(G*255.0f), int(B*255.0f), H, S, L);

	lea	eax, DWORD PTR _L$[ebp]
	push	eax
	lea	ecx, DWORD PTR _S$[ebp]
	push	ecx
	lea	edx, DWORD PTR _H$[ebp]
	push	edx
	mov	eax, DWORD PTR _B$[ebp]
	movss	xmm0, DWORD PTR [eax]
	mulss	xmm0, DWORD PTR __real@437f0000
	cvttss2si ecx, xmm0
	push	ecx
	mov	edx, DWORD PTR _G$[ebp]
	movss	xmm0, DWORD PTR [edx]
	mulss	xmm0, DWORD PTR __real@437f0000
	cvttss2si eax, xmm0
	push	eax
	mov	ecx, DWORD PTR _R$[ebp]
	movss	xmm0, DWORD PTR [ecx]
	mulss	xmm0, DWORD PTR __real@437f0000
	cvttss2si edx, xmm0
	push	edx
	call	?RGBToHSL@@YAXHHHAAH00@Z		; RGBToHSL
	add	esp, 24					; 00000018H

; 11   : 	H += amount;

	mov	eax, DWORD PTR _H$[ebp]
	add	eax, DWORD PTR _amount$[ebp]
	mov	DWORD PTR _H$[ebp], eax

; 12   : 	H %= 360;

	mov	eax, DWORD PTR _H$[ebp]
	cdq
	mov	ecx, 360				; 00000168H
	idiv	ecx
	mov	DWORD PTR _H$[ebp], edx

; 13   : 	int newR, newG, newB;
; 14   : 	HSLToRGB( H, S, L, newR, newG, newB);

	lea	eax, DWORD PTR _newB$[ebp]
	push	eax
	lea	ecx, DWORD PTR _newG$[ebp]
	push	ecx
	lea	edx, DWORD PTR _newR$[ebp]
	push	edx
	mov	eax, DWORD PTR _L$[ebp]
	push	eax
	mov	ecx, DWORD PTR _S$[ebp]
	push	ecx
	mov	edx, DWORD PTR _H$[ebp]
	push	edx
	call	?HSLToRGB@@YAXHHHAAH00@Z		; HSLToRGB
	add	esp, 24					; 00000018H

; 15   : 	R = float( newR/255.0f );

	cvtsi2ss xmm0, DWORD PTR _newR$[ebp]
	divss	xmm0, DWORD PTR __real@437f0000
	mov	eax, DWORD PTR _R$[ebp]
	movss	DWORD PTR [eax], xmm0

; 16   : 	G = float( newG/255.0f );

	cvtsi2ss xmm0, DWORD PTR _newG$[ebp]
	divss	xmm0, DWORD PTR __real@437f0000
	mov	eax, DWORD PTR _G$[ebp]
	movss	DWORD PTR [eax], xmm0

; 17   : 	B = float( newB/255.0f );

	cvtsi2ss xmm0, DWORD PTR _newB$[ebp]
	divss	xmm0, DWORD PTR __real@437f0000
	mov	eax, DWORD PTR _B$[ebp]
	movss	DWORD PTR [eax], xmm0

; 18   : }

	push	edx
	mov	ecx, ebp
	push	eax
	lea	edx, DWORD PTR $LN10@AdvanceHue
	call	@_RTC_CheckStackVars@8
	pop	eax
	pop	edx
	pop	edi
	pop	esi
	pop	ebx
	mov	ecx, DWORD PTR __$ArrayPad$[ebp]
	xor	ecx, ebp
	call	@__security_check_cookie@4
	add	esp, 268				; 0000010cH
	cmp	ebp, esp
	call	__RTC_CheckEsp
	mov	esp, ebp
	pop	ebp
	ret	0
	npad	2
$LN10@AdvanceHue:
	DD	6
	DD	$LN9@AdvanceHue
$LN9@AdvanceHue:
	DD	-12					; fffffff4H
	DD	4
	DD	$LN3@AdvanceHue
	DD	-24					; ffffffe8H
	DD	4
	DD	$LN4@AdvanceHue
	DD	-36					; ffffffdcH
	DD	4
	DD	$LN5@AdvanceHue
	DD	-48					; ffffffd0H
	DD	4
	DD	$LN6@AdvanceHue
	DD	-60					; ffffffc4H
	DD	4
	DD	$LN7@AdvanceHue
	DD	-72					; ffffffb8H
	DD	4
	DD	$LN8@AdvanceHue
$LN8@AdvanceHue:
	DB	110					; 0000006eH
	DB	101					; 00000065H
	DB	119					; 00000077H
	DB	66					; 00000042H
	DB	0
$LN7@AdvanceHue:
	DB	110					; 0000006eH
	DB	101					; 00000065H
	DB	119					; 00000077H
	DB	71					; 00000047H
	DB	0
$LN6@AdvanceHue:
	DB	110					; 0000006eH
	DB	101					; 00000065H
	DB	119					; 00000077H
	DB	82					; 00000052H
	DB	0
$LN5@AdvanceHue:
	DB	76					; 0000004cH
	DB	0
$LN4@AdvanceHue:
	DB	83					; 00000053H
	DB	0
$LN3@AdvanceHue:
	DB	72					; 00000048H
	DB	0
?AdvanceHue@@YAXHAAM00@Z ENDP				; AdvanceHue
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\visual studio 2012\projects\dissertation project\dissertation project\colours.cpp
;	COMDAT ?fHSLToRGB@@YAXMMMAAM00@Z
_TEXT	SEGMENT
tv90 = -292						; size = 4
tv73 = -292						; size = 4
_mid2$1 = -92						; size = 4
_mid1$2 = -80						; size = 4
_vsf$3 = -68						; size = 4
_fract$4 = -56						; size = 4
_sextant$5 = -44					; size = 4
_sv$6 = -32						; size = 4
_m$7 = -20						; size = 4
_v$ = -8						; size = 4
_h$ = 8							; size = 4
_sl$ = 12						; size = 4
_l$ = 16						; size = 4
_r$ = 20						; size = 4
_g$ = 24						; size = 4
_b$ = 28						; size = 4
?fHSLToRGB@@YAXMMMAAM00@Z PROC				; fHSLToRGB, COMDAT

; 167  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 292				; 00000124H
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-292]
	mov	ecx, 73					; 00000049H
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 168  :     float v;
; 169  : 
; 170  :     r = l;   // default to gray

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR [eax], xmm0

; 171  :     g = l;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR [eax], xmm0

; 172  :     b = l;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR [eax], xmm0

; 173  :     v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);

	movss	xmm0, DWORD PTR __real@3f000000
	comiss	xmm0, DWORD PTR _l$[ebp]
	jb	SHORT $LN12@fHSLToRGB
	movss	xmm0, DWORD PTR __real@3f800000
	addss	xmm0, DWORD PTR _sl$[ebp]
	mulss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR tv73[ebp], xmm0
	jmp	SHORT $LN13@fHSLToRGB
$LN12@fHSLToRGB:
	movss	xmm0, DWORD PTR _l$[ebp]
	addss	xmm0, DWORD PTR _sl$[ebp]
	movss	xmm1, DWORD PTR _l$[ebp]
	mulss	xmm1, DWORD PTR _sl$[ebp]
	subss	xmm0, xmm1
	movss	DWORD PTR tv73[ebp], xmm0
$LN13@fHSLToRGB:
	movss	xmm0, DWORD PTR tv73[ebp]
	movss	DWORD PTR _v$[ebp], xmm0

; 174  :     if (v > 0)

	movss	xmm0, DWORD PTR _v$[ebp]
	comiss	xmm0, DWORD PTR __real@00000000
	jbe	$LN7@fHSLToRGB

; 175  :     {
; 176  :             float m;
; 177  :             float sv;
; 178  :             int sextant;
; 179  :             float fract, vsf, mid1, mid2;
; 180  :  
; 181  :             m = l + l - v;

	movss	xmm0, DWORD PTR _l$[ebp]
	addss	xmm0, DWORD PTR _l$[ebp]
	subss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _m$7[ebp], xmm0

; 182  :             sv = (v - m ) / v;

	movss	xmm0, DWORD PTR _v$[ebp]
	subss	xmm0, DWORD PTR _m$7[ebp]
	divss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _sv$6[ebp], xmm0

; 183  :             h *= 6.0;

	cvtss2sd xmm0, DWORD PTR _h$[ebp]
	mulsd	xmm0, QWORD PTR __real@4018000000000000
	cvtsd2ss xmm0, xmm0
	movss	DWORD PTR _h$[ebp], xmm0

; 184  :             sextant = (int)h;

	cvttss2si eax, DWORD PTR _h$[ebp]
	mov	DWORD PTR _sextant$5[ebp], eax

; 185  :             fract = h - sextant;

	cvtsi2ss xmm0, DWORD PTR _sextant$5[ebp]
	movss	xmm1, DWORD PTR _h$[ebp]
	subss	xmm1, xmm0
	movss	DWORD PTR _fract$4[ebp], xmm1

; 186  :             vsf = v * sv * fract;

	movss	xmm0, DWORD PTR _v$[ebp]
	mulss	xmm0, DWORD PTR _sv$6[ebp]
	mulss	xmm0, DWORD PTR _fract$4[ebp]
	movss	DWORD PTR _vsf$3[ebp], xmm0

; 187  :             mid1 = m + vsf;

	movss	xmm0, DWORD PTR _m$7[ebp]
	addss	xmm0, DWORD PTR _vsf$3[ebp]
	movss	DWORD PTR _mid1$2[ebp], xmm0

; 188  :             mid2 = v - vsf;

	movss	xmm0, DWORD PTR _v$[ebp]
	subss	xmm0, DWORD PTR _vsf$3[ebp]
	movss	DWORD PTR _mid2$1[ebp], xmm0

; 189  :             switch (sextant)

	mov	eax, DWORD PTR _sextant$5[ebp]
	mov	DWORD PTR tv90[ebp], eax
	cmp	DWORD PTR tv90[ebp], 5
	ja	$LN7@fHSLToRGB
	mov	ecx, DWORD PTR tv90[ebp]
	jmp	DWORD PTR $LN14@fHSLToRGB[ecx*4]
$LN6@fHSLToRGB:

; 190  :             {
; 191  :                 case 0:
; 192  :                         r = v;

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR [eax], xmm0

; 193  :                         g = mid1;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _mid1$2[ebp]
	movss	DWORD PTR [eax], xmm0

; 194  :                         b = m;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR [eax], xmm0

; 195  :                         break;

	jmp	$LN7@fHSLToRGB
$LN5@fHSLToRGB:

; 196  :                 case 1:
; 197  :                         r = mid2;

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _mid2$1[ebp]
	movss	DWORD PTR [eax], xmm0

; 198  :                         g = v;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR [eax], xmm0

; 199  :                         b = m;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR [eax], xmm0

; 200  :                         break;

	jmp	$LN7@fHSLToRGB
$LN4@fHSLToRGB:

; 201  :                 case 2:
; 202  :                         r = m;

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR [eax], xmm0

; 203  :                         g = v;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR [eax], xmm0

; 204  :                         b = mid1;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _mid1$2[ebp]
	movss	DWORD PTR [eax], xmm0

; 205  :                         break;

	jmp	SHORT $LN7@fHSLToRGB
$LN3@fHSLToRGB:

; 206  :                 case 3:
; 207  :                         r = m;

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR [eax], xmm0

; 208  :                         g = mid2;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _mid2$1[ebp]
	movss	DWORD PTR [eax], xmm0

; 209  :                         b = v;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR [eax], xmm0

; 210  :                         break;

	jmp	SHORT $LN7@fHSLToRGB
$LN2@fHSLToRGB:

; 211  :                 case 4:
; 212  :                         r = mid1;

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _mid1$2[ebp]
	movss	DWORD PTR [eax], xmm0

; 213  :                         g = m;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR [eax], xmm0

; 214  :                         b = v;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR [eax], xmm0

; 215  :                         break;

	jmp	SHORT $LN7@fHSLToRGB
$LN1@fHSLToRGB:

; 216  :                 case 5:
; 217  :                         r = v;

	mov	eax, DWORD PTR _r$[ebp]
	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR [eax], xmm0

; 218  :                         g = m;

	mov	eax, DWORD PTR _g$[ebp]
	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR [eax], xmm0

; 219  :                         b = mid2;

	mov	eax, DWORD PTR _b$[ebp]
	movss	xmm0, DWORD PTR _mid2$1[ebp]
	movss	DWORD PTR [eax], xmm0
$LN7@fHSLToRGB:

; 220  :                         break;
; 221  :             }
; 222  :     }
; 223  :     return;
; 224  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
$LN14@fHSLToRGB:
	DD	$LN6@fHSLToRGB
	DD	$LN5@fHSLToRGB
	DD	$LN4@fHSLToRGB
	DD	$LN3@fHSLToRGB
	DD	$LN2@fHSLToRGB
	DD	$LN1@fHSLToRGB
?fHSLToRGB@@YAXMMMAAM00@Z ENDP				; fHSLToRGB
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\visual studio 2012\projects\dissertation project\dissertation project\colours.cpp
;	COMDAT ?HSLToRGB@@YAXHHHAAH00@Z
_TEXT	SEGMENT
tv128 = -364						; size = 4
tv79 = -364						; size = 4
_mid2$1 = -164						; size = 4
_mid1$2 = -152						; size = 4
_vsf$3 = -140						; size = 4
_fract$4 = -128						; size = 4
_sextant$5 = -116					; size = 4
_sv$6 = -104						; size = 4
_m$7 = -92						; size = 4
_b$ = -80						; size = 4
_g$ = -68						; size = 4
_r$ = -56						; size = 4
_v$ = -44						; size = 4
_l$ = -32						; size = 4
_sl$ = -20						; size = 4
_h$ = -8						; size = 4
_hin$ = 8						; size = 4
_slin$ = 12						; size = 4
_lin$ = 16						; size = 4
_rout$ = 20						; size = 4
_gout$ = 24						; size = 4
_bout$ = 28						; size = 4
?HSLToRGB@@YAXHHHAAH00@Z PROC				; HSLToRGB, COMDAT

; 100  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 364				; 0000016cH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-364]
	mov	ecx, 91					; 0000005bH
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 101  : 	float h = hin / 360.0f;

	cvtsi2ss xmm0, DWORD PTR _hin$[ebp]
	divss	xmm0, DWORD PTR __real@43b40000
	movss	DWORD PTR _h$[ebp], xmm0

; 102  : 	float sl = slin / 100.0f;

	cvtsi2ss xmm0, DWORD PTR _slin$[ebp]
	divss	xmm0, DWORD PTR __real@42c80000
	movss	DWORD PTR _sl$[ebp], xmm0

; 103  : 	float l = lin / 100.0f;

	cvtsi2ss xmm0, DWORD PTR _lin$[ebp]
	divss	xmm0, DWORD PTR __real@42c80000
	movss	DWORD PTR _l$[ebp], xmm0

; 104  :     float v;
; 105  :     float r,g,b;
; 106  :  
; 107  :     r = l;   // default to gray

	movss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 108  :     g = l;

	movss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 109  :     b = l;

	movss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR _b$[ebp], xmm0

; 110  :     v = (l <= 0.5f) ? (l * (1.0f + sl)) : (l + sl - l * sl);

	movss	xmm0, DWORD PTR __real@3f000000
	comiss	xmm0, DWORD PTR _l$[ebp]
	jb	SHORT $LN12@HSLToRGB
	movss	xmm0, DWORD PTR __real@3f800000
	addss	xmm0, DWORD PTR _sl$[ebp]
	mulss	xmm0, DWORD PTR _l$[ebp]
	movss	DWORD PTR tv79[ebp], xmm0
	jmp	SHORT $LN13@HSLToRGB
$LN12@HSLToRGB:
	movss	xmm0, DWORD PTR _l$[ebp]
	addss	xmm0, DWORD PTR _sl$[ebp]
	movss	xmm1, DWORD PTR _l$[ebp]
	mulss	xmm1, DWORD PTR _sl$[ebp]
	subss	xmm0, xmm1
	movss	DWORD PTR tv79[ebp], xmm0
$LN13@HSLToRGB:
	movss	xmm0, DWORD PTR tv79[ebp]
	movss	DWORD PTR _v$[ebp], xmm0

; 111  :     if (v > 0)

	movss	xmm0, DWORD PTR _v$[ebp]
	comiss	xmm0, DWORD PTR __real@00000000
	jbe	$LN7@HSLToRGB

; 112  :     {
; 113  :             float m;
; 114  :             float sv;
; 115  :             int sextant;
; 116  :             float fract, vsf, mid1, mid2;
; 117  :  
; 118  :             m = l + l - v;

	movss	xmm0, DWORD PTR _l$[ebp]
	addss	xmm0, DWORD PTR _l$[ebp]
	subss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _m$7[ebp], xmm0

; 119  :             sv = (v - m ) / v;

	movss	xmm0, DWORD PTR _v$[ebp]
	subss	xmm0, DWORD PTR _m$7[ebp]
	divss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _sv$6[ebp], xmm0

; 120  :             h *= 6.0;

	cvtss2sd xmm0, DWORD PTR _h$[ebp]
	mulsd	xmm0, QWORD PTR __real@4018000000000000
	cvtsd2ss xmm0, xmm0
	movss	DWORD PTR _h$[ebp], xmm0

; 121  :             sextant = (int)h;

	cvttss2si eax, DWORD PTR _h$[ebp]
	mov	DWORD PTR _sextant$5[ebp], eax

; 122  :             fract = h - sextant;

	cvtsi2ss xmm0, DWORD PTR _sextant$5[ebp]
	movss	xmm1, DWORD PTR _h$[ebp]
	subss	xmm1, xmm0
	movss	DWORD PTR _fract$4[ebp], xmm1

; 123  :             vsf = v * sv * fract;

	movss	xmm0, DWORD PTR _v$[ebp]
	mulss	xmm0, DWORD PTR _sv$6[ebp]
	mulss	xmm0, DWORD PTR _fract$4[ebp]
	movss	DWORD PTR _vsf$3[ebp], xmm0

; 124  :             mid1 = m + vsf;

	movss	xmm0, DWORD PTR _m$7[ebp]
	addss	xmm0, DWORD PTR _vsf$3[ebp]
	movss	DWORD PTR _mid1$2[ebp], xmm0

; 125  :             mid2 = v - vsf;

	movss	xmm0, DWORD PTR _v$[ebp]
	subss	xmm0, DWORD PTR _vsf$3[ebp]
	movss	DWORD PTR _mid2$1[ebp], xmm0

; 126  :             switch (sextant)

	mov	eax, DWORD PTR _sextant$5[ebp]
	mov	DWORD PTR tv128[ebp], eax
	cmp	DWORD PTR tv128[ebp], 5
	ja	$LN7@HSLToRGB
	mov	ecx, DWORD PTR tv128[ebp]
	jmp	DWORD PTR $LN14@HSLToRGB[ecx*4]
$LN6@HSLToRGB:

; 127  :             {
; 128  :                 case 0:
; 129  :                         r = v;

	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 130  :                         g = mid1;

	movss	xmm0, DWORD PTR _mid1$2[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 131  :                         b = m;

	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR _b$[ebp], xmm0

; 132  :                         break;

	jmp	$LN7@HSLToRGB
$LN5@HSLToRGB:

; 133  :                 case 1:
; 134  :                         r = mid2;

	movss	xmm0, DWORD PTR _mid2$1[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 135  :                         g = v;

	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 136  :                         b = m;

	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR _b$[ebp], xmm0

; 137  :                         break;

	jmp	$LN7@HSLToRGB
$LN4@HSLToRGB:

; 138  :                 case 2:
; 139  :                         r = m;

	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 140  :                         g = v;

	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 141  :                         b = mid1;

	movss	xmm0, DWORD PTR _mid1$2[ebp]
	movss	DWORD PTR _b$[ebp], xmm0

; 142  :                         break;

	jmp	SHORT $LN7@HSLToRGB
$LN3@HSLToRGB:

; 143  :                 case 3:
; 144  :                         r = m;

	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 145  :                         g = mid2;

	movss	xmm0, DWORD PTR _mid2$1[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 146  :                         b = v;

	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _b$[ebp], xmm0

; 147  :                         break;

	jmp	SHORT $LN7@HSLToRGB
$LN2@HSLToRGB:

; 148  :                 case 4:
; 149  :                         r = mid1;

	movss	xmm0, DWORD PTR _mid1$2[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 150  :                         g = m;

	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 151  :                         b = v;

	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _b$[ebp], xmm0

; 152  :                         break;

	jmp	SHORT $LN7@HSLToRGB
$LN1@HSLToRGB:

; 153  :                 case 5:
; 154  :                         r = v;

	movss	xmm0, DWORD PTR _v$[ebp]
	movss	DWORD PTR _r$[ebp], xmm0

; 155  :                         g = m;

	movss	xmm0, DWORD PTR _m$7[ebp]
	movss	DWORD PTR _g$[ebp], xmm0

; 156  :                         b = mid2;

	movss	xmm0, DWORD PTR _mid2$1[ebp]
	movss	DWORD PTR _b$[ebp], xmm0
$LN7@HSLToRGB:

; 157  :                         break;
; 158  :             }
; 159  :     }
; 160  : 	rout = int( r * 255.0f );

	movss	xmm0, DWORD PTR _r$[ebp]
	mulss	xmm0, DWORD PTR __real@437f0000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _rout$[ebp]
	mov	DWORD PTR [ecx], eax

; 161  : 	bout = int( b * 255.0f );

	movss	xmm0, DWORD PTR _b$[ebp]
	mulss	xmm0, DWORD PTR __real@437f0000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _bout$[ebp]
	mov	DWORD PTR [ecx], eax

; 162  : 	gout = int( g * 255.0f );

	movss	xmm0, DWORD PTR _g$[ebp]
	mulss	xmm0, DWORD PTR __real@437f0000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _gout$[ebp]
	mov	DWORD PTR [ecx], eax

; 163  :     return;
; 164  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
	npad	1
$LN14@HSLToRGB:
	DD	$LN6@HSLToRGB
	DD	$LN5@HSLToRGB
	DD	$LN4@HSLToRGB
	DD	$LN3@HSLToRGB
	DD	$LN2@HSLToRGB
	DD	$LN1@HSLToRGB
?HSLToRGB@@YAXHHHAAH00@Z ENDP				; HSLToRGB
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\visual studio 2012\projects\dissertation project\dissertation project\colours.cpp
;	COMDAT ?RGBToHSL@@YAXHHHAAH00@Z
_TEXT	SEGMENT
_fMin$ = -56						; size = 4
_fMax$ = -44						; size = 4
_fB$ = -32						; size = 4
_fG$ = -20						; size = 4
_fR$ = -8						; size = 4
_R$ = 8							; size = 4
_G$ = 12						; size = 4
_B$ = 16						; size = 4
_H$ = 20						; size = 4
_S$ = 24						; size = 4
_L$ = 28						; size = 4
?RGBToHSL@@YAXHHHAAH00@Z PROC				; RGBToHSL, COMDAT

; 52   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 252				; 000000fcH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-252]
	mov	ecx, 63					; 0000003fH
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 53   : 	// Fill in the correct code here for question 4, the functions Min and Max above will help
; 54   : 	float fR, fG, fB, fMax, fMin;
; 55   : 	
; 56   : 	//convert int to float 0-1
; 57   : 	fR = (float) R / 255.0f;

	cvtsi2ss xmm0, DWORD PTR _R$[ebp]
	divss	xmm0, DWORD PTR __real@437f0000
	movss	DWORD PTR _fR$[ebp], xmm0

; 58   : 	fG = (float) G / 255.0f;

	cvtsi2ss xmm0, DWORD PTR _G$[ebp]
	divss	xmm0, DWORD PTR __real@437f0000
	movss	DWORD PTR _fG$[ebp], xmm0

; 59   : 	fB = (float) B / 255.0f;

	cvtsi2ss xmm0, DWORD PTR _B$[ebp]
	divss	xmm0, DWORD PTR __real@437f0000
	movss	DWORD PTR _fB$[ebp], xmm0

; 60   : 
; 61   : 	//find max and min values
; 62   : 	fMin = Min(fR, fG, fB);

	push	ecx
	movss	xmm0, DWORD PTR _fB$[ebp]
	movss	DWORD PTR [esp], xmm0
	push	ecx
	movss	xmm0, DWORD PTR _fG$[ebp]
	movss	DWORD PTR [esp], xmm0
	push	ecx
	movss	xmm0, DWORD PTR _fR$[ebp]
	movss	DWORD PTR [esp], xmm0
	call	?Min@@YAMMMM@Z				; Min
	add	esp, 12					; 0000000cH
	fstp	DWORD PTR _fMin$[ebp]

; 63   : 	fMax = Max(fR, fG, fB);

	push	ecx
	movss	xmm0, DWORD PTR _fB$[ebp]
	movss	DWORD PTR [esp], xmm0
	push	ecx
	movss	xmm0, DWORD PTR _fG$[ebp]
	movss	DWORD PTR [esp], xmm0
	push	ecx
	movss	xmm0, DWORD PTR _fR$[ebp]
	movss	DWORD PTR [esp], xmm0
	call	?Max@@YAMMMM@Z				; Max
	add	esp, 12					; 0000000cH
	fstp	DWORD PTR _fMax$[ebp]

; 64   : 
; 65   : 
; 66   : 	L = int( 50 * (fMax +fMin) );

	movss	xmm0, DWORD PTR _fMax$[ebp]
	addss	xmm0, DWORD PTR _fMin$[ebp]
	mulss	xmm0, DWORD PTR __real@42480000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _L$[ebp]
	mov	DWORD PTR [ecx], eax

; 67   : 	
; 68   : 	//greys - catch div/0 error
; 69   : 	if(fMax == fMin){

	movss	xmm0, DWORD PTR _fMax$[ebp]
	ucomiss	xmm0, DWORD PTR _fMin$[ebp]
	lahf
	test	ah, 68					; 00000044H
	jp	SHORT $LN8@RGBToHSL

; 70   : 		H=0;

	mov	eax, DWORD PTR _H$[ebp]
	mov	DWORD PTR [eax], 0

; 71   : 		S=0;

	mov	eax, DWORD PTR _S$[ebp]
	mov	DWORD PTR [eax], 0

; 72   : 		 return;

	jmp	$LN9@RGBToHSL
$LN8@RGBToHSL:

; 73   : 	}
; 74   : 
; 75   : 	if(L < 50){

	mov	eax, DWORD PTR _L$[ebp]
	cmp	DWORD PTR [eax], 50			; 00000032H
	jge	SHORT $LN7@RGBToHSL

; 76   : 		S = int ( 100 * (fMax - fMin) / (fMax + fMin) );

	movss	xmm0, DWORD PTR _fMax$[ebp]
	subss	xmm0, DWORD PTR _fMin$[ebp]
	mulss	xmm0, DWORD PTR __real@42c80000
	movss	xmm1, DWORD PTR _fMax$[ebp]
	addss	xmm1, DWORD PTR _fMin$[ebp]
	divss	xmm0, xmm1
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _S$[ebp]
	mov	DWORD PTR [ecx], eax

; 77   : 	}else{

	jmp	SHORT $LN6@RGBToHSL
$LN7@RGBToHSL:

; 78   : 		S = int (100 * (fMax - fMin) / (2.0 - (fMax + fMin)) );

	movss	xmm0, DWORD PTR _fMax$[ebp]
	subss	xmm0, DWORD PTR _fMin$[ebp]
	mulss	xmm0, DWORD PTR __real@42c80000
	cvtss2sd xmm0, xmm0
	movss	xmm1, DWORD PTR _fMax$[ebp]
	addss	xmm1, DWORD PTR _fMin$[ebp]
	cvtss2sd xmm1, xmm1
	movsd	xmm2, QWORD PTR __real@4000000000000000
	subsd	xmm2, xmm1
	divsd	xmm0, xmm2
	cvttsd2si eax, xmm0
	mov	ecx, DWORD PTR _S$[ebp]
	mov	DWORD PTR [ecx], eax
$LN6@RGBToHSL:

; 79   : 	}
; 80   : 
; 81   : 
; 82   : 	if(fMax == fR){

	movss	xmm0, DWORD PTR _fMax$[ebp]
	ucomiss	xmm0, DWORD PTR _fR$[ebp]
	lahf
	test	ah, 68					; 00000044H
	jp	SHORT $LN5@RGBToHSL

; 83   : 		H = int ( 60 * ( (fG - fB) / (fMax - fMin) ) );

	movss	xmm0, DWORD PTR _fG$[ebp]
	subss	xmm0, DWORD PTR _fB$[ebp]
	movss	xmm1, DWORD PTR _fMax$[ebp]
	subss	xmm1, DWORD PTR _fMin$[ebp]
	divss	xmm0, xmm1
	mulss	xmm0, DWORD PTR __real@42700000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _H$[ebp]
	mov	DWORD PTR [ecx], eax
	jmp	SHORT $LN2@RGBToHSL
$LN5@RGBToHSL:

; 84   : 	}else if(fMax == fG){

	movss	xmm0, DWORD PTR _fMax$[ebp]
	ucomiss	xmm0, DWORD PTR _fG$[ebp]
	lahf
	test	ah, 68					; 00000044H
	jp	SHORT $LN3@RGBToHSL

; 85   : 		H = int ( 60 * ( (fB - fR) / (fMax - fMin) ) + 120 );

	movss	xmm0, DWORD PTR _fB$[ebp]
	subss	xmm0, DWORD PTR _fR$[ebp]
	movss	xmm1, DWORD PTR _fMax$[ebp]
	subss	xmm1, DWORD PTR _fMin$[ebp]
	divss	xmm0, xmm1
	mulss	xmm0, DWORD PTR __real@42700000
	addss	xmm0, DWORD PTR __real@42f00000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _H$[ebp]
	mov	DWORD PTR [ecx], eax

; 86   : 	}else{

	jmp	SHORT $LN2@RGBToHSL
$LN3@RGBToHSL:

; 87   : 		H = int ( 60 * ( (fR - fG) / (fMax - fMin) ) + 240 );

	movss	xmm0, DWORD PTR _fR$[ebp]
	subss	xmm0, DWORD PTR _fG$[ebp]
	movss	xmm1, DWORD PTR _fMax$[ebp]
	subss	xmm1, DWORD PTR _fMin$[ebp]
	divss	xmm0, xmm1
	mulss	xmm0, DWORD PTR __real@42700000
	addss	xmm0, DWORD PTR __real@43700000
	cvttss2si eax, xmm0
	mov	ecx, DWORD PTR _H$[ebp]
	mov	DWORD PTR [ecx], eax
$LN2@RGBToHSL:

; 88   : 	}
; 89   : 
; 90   : 	if(H < 0){

	mov	eax, DWORD PTR _H$[ebp]
	cmp	DWORD PTR [eax], 0
	jge	SHORT $LN1@RGBToHSL

; 91   : 		H += 360;

	mov	eax, DWORD PTR _H$[ebp]
	mov	ecx, DWORD PTR [eax]
	add	ecx, 360				; 00000168H
	mov	edx, DWORD PTR _H$[ebp]
	mov	DWORD PTR [edx], ecx
$LN1@RGBToHSL:
$LN9@RGBToHSL:

; 92   : 	}
; 93   : 	return;
; 94   : 
; 95   : }

	pop	edi
	pop	esi
	pop	ebx
	add	esp, 252				; 000000fcH
	cmp	ebp, esp
	call	__RTC_CheckEsp
	mov	esp, ebp
	pop	ebp
	ret	0
?RGBToHSL@@YAXHHHAAH00@Z ENDP				; RGBToHSL
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\visual studio 2012\projects\dissertation project\dissertation project\colours.cpp
;	COMDAT ?Max@@YAMMMM@Z
_TEXT	SEGMENT
_fMax$ = -8						; size = 4
_f1$ = 8						; size = 4
_f2$ = 12						; size = 4
_f3$ = 16						; size = 4
?Max@@YAMMMM@Z PROC					; Max, COMDAT

; 37   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 204				; 000000ccH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-204]
	mov	ecx, 51					; 00000033H
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 38   : 	float fMax = f1;

	movss	xmm0, DWORD PTR _f1$[ebp]
	movss	DWORD PTR _fMax$[ebp], xmm0

; 39   : 	if (f2 > fMax)

	movss	xmm0, DWORD PTR _f2$[ebp]
	comiss	xmm0, DWORD PTR _fMax$[ebp]
	jbe	SHORT $LN2@Max

; 40   : 	{
; 41   : 		fMax = f2;

	movss	xmm0, DWORD PTR _f2$[ebp]
	movss	DWORD PTR _fMax$[ebp], xmm0
$LN2@Max:

; 42   : 	}
; 43   : 	if (f3 > fMax)

	movss	xmm0, DWORD PTR _f3$[ebp]
	comiss	xmm0, DWORD PTR _fMax$[ebp]
	jbe	SHORT $LN1@Max

; 44   : 	{
; 45   : 		fMax = f3;

	movss	xmm0, DWORD PTR _f3$[ebp]
	movss	DWORD PTR _fMax$[ebp], xmm0
$LN1@Max:

; 46   : 	}
; 47   : 	return fMax;

	fld	DWORD PTR _fMax$[ebp]

; 48   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Max@@YAMMMM@Z ENDP					; Max
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\visual studio 2012\projects\dissertation project\dissertation project\colours.cpp
;	COMDAT ?Min@@YAMMMM@Z
_TEXT	SEGMENT
_fMin$ = -8						; size = 4
_f1$ = 8						; size = 4
_f2$ = 12						; size = 4
_f3$ = 16						; size = 4
?Min@@YAMMMM@Z PROC					; Min, COMDAT

; 22   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 204				; 000000ccH
	push	ebx
	push	esi
	push	edi
	lea	edi, DWORD PTR [ebp-204]
	mov	ecx, 51					; 00000033H
	mov	eax, -858993460				; ccccccccH
	rep stosd

; 23   : 	float fMin = f1;

	movss	xmm0, DWORD PTR _f1$[ebp]
	movss	DWORD PTR _fMin$[ebp], xmm0

; 24   : 	if (f2 < fMin)

	movss	xmm0, DWORD PTR _fMin$[ebp]
	comiss	xmm0, DWORD PTR _f2$[ebp]
	jbe	SHORT $LN2@Min

; 25   : 	{
; 26   : 		fMin = f2;

	movss	xmm0, DWORD PTR _f2$[ebp]
	movss	DWORD PTR _fMin$[ebp], xmm0
$LN2@Min:

; 27   : 	}
; 28   : 	if (f3 < fMin)

	movss	xmm0, DWORD PTR _fMin$[ebp]
	comiss	xmm0, DWORD PTR _f3$[ebp]
	jbe	SHORT $LN1@Min

; 29   : 	{
; 30   : 		fMin = f3;

	movss	xmm0, DWORD PTR _f3$[ebp]
	movss	DWORD PTR _fMin$[ebp], xmm0
$LN1@Min:

; 31   : 	}
; 32   : 	return fMin;

	fld	DWORD PTR _fMin$[ebp]

; 33   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Min@@YAMMMM@Z ENDP					; Min
_TEXT	ENDS
END
