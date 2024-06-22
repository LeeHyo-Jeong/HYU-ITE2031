			lw 0 1 one
			lw 0 2 ten
			lw 0 3 zero
loop		add	3 1 3
			add 1 1 1
			beq 1 2 end
			beq 0 0 loop
end		halt
zero		.fill 0
one		.fill 1
ten		.fill 10
