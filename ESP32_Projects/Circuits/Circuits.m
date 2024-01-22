V1 = 10;
R1 = 10;
R2 = 20;
R3 = 40;
R4 = 20;

R234 = (R2+R4)*(R3) / (R2 + R4 + R3)
Req = R234 + R1

I = V1/Req

I1 = I

V2 = V1 - I1*R1

I3 = V2/R3

I2 = V2/(R2 + R4)

I4 = V2/(R2 + R4)

P1 = I1^2 * R1;
P2 = I2^2 * R2;
P3 = I3^2 * R3;
P4 = I4^2 * R4;

P = [P1, P2, P3, P4]

bar(P), xlabel('Power'), ylabel('Watts'), title('Power of Resistors');
