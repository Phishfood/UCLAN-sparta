; Listing generated by Microsoft (R) Optimizing Compiler Version 18.00.30501.0 

	TITLE	C:\Users\Simon\Documents\GitHub\Sparta2.0\UCLAN-sparta\AIRandom.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMTD
INCLUDELIB OLDNAMES

PUBLIC	??0CAIRandom@@QAE@XZ				; CAIRandom::CAIRandom
PUBLIC	??1CAIRandom@@QAE@XZ				; CAIRandom::~CAIRandom
EXTRN	__RTC_InitBase:PROC
EXTRN	__RTC_Shutdown:PROC
;	COMDAT rtc$TMZ
rtc$TMZ	SEGMENT
__RTC_Shutdown.rtc$TMZ DD FLAT:__RTC_Shutdown
rtc$TMZ	ENDS
;	COMDAT rtc$IMZ
rtc$IMZ	SEGMENT
__RTC_InitBase.rtc$IMZ DD FLAT:__RTC_InitBase
rtc$IMZ	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\github\sparta2.0\uclan-sparta\airandom.cpp
;	COMDAT ??1CAIRandom@@QAE@XZ
_TEXT	SEGMENT
_this$ = -8						; size = 4
??1CAIRandom@@QAE@XZ PROC				; CAIRandom::~CAIRandom, COMDAT
; _this$ = ecx

; 10   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 204				; 000000ccH
	push	ebx
	push	esi
	push	edi
	push	ecx
	lea	edi, DWORD PTR [ebp-204]
	mov	ecx, 51					; 00000033H
	mov	eax, -858993460				; ccccccccH
	rep stosd
	pop	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 11   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
??1CAIRandom@@QAE@XZ ENDP				; CAIRandom::~CAIRandom
_TEXT	ENDS
; Function compile flags: /Odtp /RTCsu /ZI
; File c:\users\simon\documents\github\sparta2.0\uclan-sparta\airandom.cpp
;	COMDAT ??0CAIRandom@@QAE@XZ
_TEXT	SEGMENT
_this$ = -8						; size = 4
??0CAIRandom@@QAE@XZ PROC				; CAIRandom::CAIRandom, COMDAT
; _this$ = ecx

; 5    : {

	push	ebp
	mov	ebp, esp
	sub	esp, 204				; 000000ccH
	push	ebx
	push	esi
	push	edi
	push	ecx
	lea	edi, DWORD PTR [ebp-204]
	mov	ecx, 51					; 00000033H
	mov	eax, -858993460				; ccccccccH
	rep stosd
	pop	ecx
	mov	DWORD PTR _this$[ebp], ecx

; 6    : }

	mov	eax, DWORD PTR _this$[ebp]
	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
??0CAIRandom@@QAE@XZ ENDP				; CAIRandom::CAIRandom
_TEXT	ENDS
END
