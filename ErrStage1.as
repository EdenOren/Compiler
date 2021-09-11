;this file contain errors for stage 1

;commas
and ,$4, $5, $6
or    $10 . $11,$12 
nor $4,$5,$6	,
nori $11,, +50000, $1
or       $7 $8, $9

; parameters
add     $1 ,$2  
sub $12, $1, $2, $21
and $32 ,$16, $8
or      $5 ,10,$15
nor     $1,$+1,$1 
move    $1 0,$10
mvhi    $5		,	$15$
mvlo    $0, what
addi    $1  , 2, 3
subi    4 ,   5   ,   $6
andi    $7,    $8,    $9
bne   5 ,   $5,            label
beq           $1,              validated         , $2 
blt          $ 4,    $4,        onstage2
bgt 	$1, $2, asciz
lb      $1  ,  50000,  $33    
sb      $1 ,   $1   ,   $1
lw      -5,    5,    $5
sw      $31,-+13,$31 
lh      $17,71,7
sh      1$ ,   0   ,   $3 
jmp     1
la	3$
call		$5
stop .

.dh 100, 10, 0, -10, -100.1
.db .dh, 1, 10, 15
.dw 10000000000000, +-1, 300

; asciz
.asciz "OutsideRight"		 	,
.asciz  	 	~"OutsideLeft"
.asciz Missing Quotes
.asciz Missing Quote Start"
.asciz "Missing Quote End

; not directive / instruction
STOP
not_a_cmd ?!
db 1, 5, 10, 1600
.nori $1, $5, $7

ThisLabelExactlyLengthOf31Nowww: and $4, $5 ,$6
doubles: add $1, $2, $3
Ambiguous: add $0, $1, $2

; labels
K move $1, $5
add: .add CantUseSaveWord
$ogood2: .dw 10 , 40 	
not,good: .db 1
ThisLabelIsTooLongThisLabelIsTooLong: and $4, $5 ,$6
DoubleLabel: Another: add $0 , $1 , $2
doubles: .dh 1, 2, 3
.extern Ambiguous

LineMuchTooTooTooLongLongLongLineMuchTooTooTooLongLongLongLineMuchTooTooTooLongLqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
.asciz " What A Long Line What A Long Line What A Long Line What A Long Line !! "
.db 1,2,3,4,5,6,7,89,10,11,12,14,15,16,17,100000000000000000000000000000000000011
LineNotMuchSize80TooLongLongLongLineMuchSize80TooLongLongLongSize80MuchSize80Too