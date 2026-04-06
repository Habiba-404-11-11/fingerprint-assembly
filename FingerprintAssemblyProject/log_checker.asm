.MODEL SMALL
.STACK 100h
.DATA
filename db 'LOG.txt',0
buffer   db 128 dup(0)
newline  db 0Dh,0Ah,0

grantMsg db 'access granted on id ',0
denyMsg  db 'access denied on id ',0

.CODE

; -------------------------
; READ_LINE
; -------------------------
READ_LINE PROC
    MOV SI,0
READ_CHAR:
    MOV AH,3Fh
    MOV CX,1
    LEA DX,buffer+SI
    INT 21h
    JC @EOF
    CMP AX,0
    JE @EOF
    MOV AL,buffer+SI
    CMP AL,0Ah
    JE @ENDLINE
    CMP AL,0Dh
    JE READ_CHAR
    INC SI
    JMP READ_CHAR
@ENDLINE:
    MOV buffer[SI],0
    MOV AX,1
    RET
@EOF:
    MOV AX,0
    RET
READ_LINE ENDP

; -------------------------
; MAIN
; -------------------------
MAIN PROC
    MOV AX,@DATA
    MOV DS,AX

    ; Open file
    MOV AH,3Dh
    MOV AL,0
    LEA DX,filename
    INT 21h
    JC EXIT
    MOV BX,AX

READ_LOOP:
    CALL READ_LINE
    CMP AX,0
    JE CLOSE_FILE

    ; ===== CHECK FOR 50 OR 60 =====
    MOV SI,0
CHECK_LINE:
    MOV AL,buffer[SI]
    CMP AL,0
    JE PRINT_LINE

    CMP AL,'5'
    JE CHECK_50
    CMP AL,'6'
    JE CHECK_60

    INC SI
    JMP CHECK_LINE

CHECK_50:
    CMP buffer[SI+1],'0'
    JE ACCESS_GRANTED
    INC SI
    JMP CHECK_LINE

CHECK_60:
    CMP buffer[SI+1],'0'
    JE ACCESS_DENIED
    INC SI
    JMP CHECK_LINE

; ===== ACCESS GRANTED =====
ACCESS_GRANTED:
    MOV DI,0
PRINT_GRANT:
    MOV AL,grantMsg[DI]
    CMP AL,0
    JE PRINT_ID_ONLY_GRANT
    MOV AH,02h
    MOV DL,AL
    INT 21h
    INC DI
    JMP PRINT_GRANT

PRINT_ID_ONLY_GRANT:
    ADD SI,2          ; skip 50
PRINT_ID_LOOP_G:
    MOV AL,buffer[SI]
    CMP AL,0
    JE PRINT_NEWLINE
    MOV AH,02h
    MOV DL,AL
    INT 21h
    INC SI
    JMP PRINT_ID_LOOP_G

; ===== ACCESS DENIED =====
ACCESS_DENIED:
    MOV DI,0
PRINT_DENY:
    MOV AL,denyMsg[DI]
    CMP AL,0
    JE PRINT_ID_ONLY_DENY
    MOV AH,02h
    MOV DL,AL
    INT 21h
    INC DI
    JMP PRINT_DENY

PRINT_ID_ONLY_DENY:
    ADD SI,2          ; skip 60
PRINT_ID_LOOP_D:
    MOV AL,buffer[SI]
    CMP AL,0
    JE PRINT_NEWLINE
    MOV AH,02h
    MOV DL,AL
    INT 21h
    INC SI
    JMP PRINT_ID_LOOP_D

; ===== NORMAL LINE PRINT =====
PRINT_LINE:
    MOV SI,0
PRINT_LINE_LOOP:
    MOV AL,buffer[SI]
    CMP AL,0
    JE PRINT_NEWLINE
    MOV AH,02h
    MOV DL,AL
    INT 21h
    INC SI
    JMP PRINT_LINE_LOOP

PRINT_NEWLINE:
    MOV SI,0
PRINT_NL:
    MOV AL,newline[SI]
    CMP AL,0
    JE READ_LOOP
    MOV AH,02h
    MOV DL,AL
    INT 21h
    INC SI
    JMP PRINT_NL

CLOSE_FILE:
    MOV AH,3Eh
    INT 21h

EXIT:
    MOV AH,4Ch
    INT 21h
MAIN ENDP
END MAIN
