import matplotlib.pyplot as plt;

V1 = 10;
R1 = 5;
R2 = 20;
R3 = 15;
R4 = 20;


R234 = (R2+R4)*(R3) / (R2 + R4 + R3)
Req = R234 + R1


I = V1/Req

I1 = I

V2 = V1 - I1*R1

I3 = V2/R3

I2 = V2/(R2 + R4)

I4 = V2/(R2 + R4)


P1 = I1**2 * R1;
P2 = I2**2 * R2;
P3 = I3**2 * R3;
P4 = I4**2 * R4;

print('I1 = {}, I2 = {}, I3 = {}, I4 = {}'.format(I1, I2, I3, I4))
print('P1 = {}, P2 = {}, P3 = {}, P4 = {}'.format(P1, P2, P3, P4))

P_title = ['P1', 'P2', 'P3', 'P4']
P = [P1, P2, P3, P4]

plt.bar(P_title, P)

plt.xlabel('Power')
plt.ylabel('Watts')
  
plt.title('Power of Resistors')
  
plt.show()
