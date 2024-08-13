-------------MEMORY
00000 000013 // N - espaço pra input do usuario
00001 000000 // FIB1 - primeiro numero da sequencia
00002 000001 // FIB2 - segundo numero da sequencia
00003 000000 // TEMP - reserva temporaria para adicao
00004 000000 // COUNT- contador de loop
00005 000000 // NEXT - proximo numero da sequencia (fib1 + fib2)
00006 000022 // SEQ_LOC - endereço atual pra guardar o next (atualizado a cada iteracao)
00007 777776 // -2 - constante 
00010 777777 // -1 - constante

00020 000000 // SEQ  - endereco de inicio pra guardar a sequencia de fibonacci calculada
00021 000000 // SEQ+1

--------------PROGRAM
00000 750000 //CLA - clear AC
00001 750000 // CLA     - clear AC - nop
00002 200000 // LAC 0   - AC<-N
00003 741200 // SNA     - pula se AC != 0
00004 600100 // JMP END - se AC==0, termina o programa (jump para END)
00005 200001 // LAC 1   - AC<-FIB1 (0)
00006 040020 // DAC 20  - SEQ<-FIB1
00007 200002 // LAC 2   - AC<-FIB2
00010 040021 // DAC 21  - SEQ+1<-FIB2
00011 200000 // LAC 0   - AC<-N
00012 340007 // TAD 7   - AC<-N-2   
00013 040004 // DAC 4   - COUNT<-N-2
// LOOP
00014 200004 // LAC 4        - AC<-COUNT
00015 741200 // SNA          - pula se COUNT != 0
00016 600100 // JMP END      - se COUNT==0, termina o programa
00017 600031 // JMP FIB_CALC - calcula o proximo numero da sequencia - subrotina
00020 200004 // LAC 4        - AC<-COUNT
00021 340010 // TAD 10       - AC<-COUNT-1
00022 040004 // DAC 4
00023 600014 // JMP LOOP     - repete loop


// FIB_CALC - subrotina para calcular o proximo fibonacci e atualizar os valores de fib1, fib2, next, seq_loc
00030 750000 // CLA     - clear AC
00031 200002 // LAC 2   - AC<-FIB2
00032 040003 // DAC 3   - TEMP<-FIB2
00033 340001 // TAD 1   - AC<-AC+FIB1
00034 040005 // DAC 5   - NEXT<-FIB2+FIB1
00035 200003 // LAC 3   - AC<-TEMP
00036 040001 // DAC 1   - FIB1<-TEMP
00037 200005 // LAC 5   - AC<-NEXT
00040 040002 // DAC 2   - FIB2<-NEXT
00041 060006 // DAC I 6 - [SEQ_LOC]<-NEXT
00042 440006 // ISZ 6   - SEQ_LOC<-SEQ_LOC+1
00043 200005
00044 700406
00045 600020 // JMP 20  - retorna da subrotina

// END - halt
00100 740040