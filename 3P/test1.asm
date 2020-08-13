

# execution will start here (at address 0)
export sum
export top
export done

jmp skipData
sum:
  word 0
len:
  word 5
vector:
  word 1
  word 2
  word 3
  word 4
  word 5
skipData:
  ldimm  r0, 0 
  load   r1, len
  ldaddr r2, vector
  ldimm  r3, 0 
  