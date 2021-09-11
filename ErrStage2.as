;this file contain errors for stage 2

.extern NotForBranch

blt $1, $2, NotForBranch
beq $3, $4, NotForBranch
bgt $29, $30, NotForBranch
bne $31, $0, NotForBranch

.entry Missing

bgt 	$20,$2,  	Missing
bgt $11, $31, Undefined
beq	$5, $30, Missing
jmp Unlabeled
call Unlabeled
la Missing

.extern NotForEntry
.entry NotForEntry