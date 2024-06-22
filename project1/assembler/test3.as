		lw 0 1 one
		lw 0 2 two
		lw 0 3 three
		lw 0 4 zero
loop	add 4 2 4
		add 1 2 1
		beq 1 3 end
		beq 0 0 loop
end	halt
zero	.fill 0
one	.fill 1
two	.fill 2
three	.fill 3
