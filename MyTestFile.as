; extern before instruction
.extern before

; unused extern is not printed in extern output file.
.extern unused

; same operand
andi $10, -1, $10

; conditional branch to label
blt $5, $24,				defLater

; extern double decleration
.extern doubledeclare

; jmp to register
validLabel: jmp $1

; label is disregarded before entry/extern
NoNeed: .entry validLabel
$novalid: .extern xXx

defLater: lh $9, 0, $29

; jmp to label
jmp doubledeclare

; extern after instruction
la before
.extern doubledeclare

; label name similar saved word
asciZ: stop

.dw -1
STR1NG: .asciz "abc"
add $0,$3, $31
bgt	$5, $10, validLabel

.entry OneTo8
.entry dh1
.entry dw2

; same operand
andi $10, -1, $10

; out of range value 2nd complement
andi $10, +65536, $10
OneTo8: .db 1, 2, 3, 4, 	5, 6, 7, 520
db3: .db 8589934592
dh1: .dh -8589934592
dw2: .dw 8589934592

add     $1 ,$2  , $3
sub $0, $0, $0
and $31 ,$30, $0
or      $5 ,$10, $15
nor     $1,$1,$1 
move    $10,$10
mvhi    $5		,	$15
mvlo    $0, $1
addi    $1  , 2, $3
subi    $4 ,   -10000000000 ,   $6
andi    $7,    +230000,    $9
bne   $5 ,   $5,            dh1
beq           $1,              $2 , db3
blt          $4,    $4,        dw2
bgt 	$1, $2, validLabel
lb      $1  ,  50000,  $31    
sb      $1 ,   1   ,   $1
	 	lw     	 $5,    5,   	 	  $5
		          sw      $31,-13,$31 
lh      $17,71,$7
sh      $1 ,   0   ,   $3 
jmp     $1
jmp validLabel
la	validLabel
call		validLabel
stop

; asciz
.asciz "~!@#@$^$*"""""""validLabel""""""")(&*&^%#$@!"
.asciz  	 	"                                                  "
.asciz "Reee34657945365afadfssssa2342eeally Long, like 80 chars asciz directive"
		.asciz 	 "~~~~!Missing Quote Start"
.asciz " !Missing Quote End                                          "
