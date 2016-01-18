/* { dg-do run } */
/* PR target/58574 */

__attribute__((noinline, noclone)) double
foo (double x)
{
  double t;
  switch ((int) x)
    {
    case 0:
      t = 2 * x - 1;
      return 0.70878e-3 + (0.71234e-3 + (0.35779e-5 + (0.17403e-7 + (0.81710e-10 + (0.36885e-12 + 0.15917e-14 * t) * t) * t) * t) * t) * t;
    case 1:
      t = 2 * x - 3;
      return 0.21479e-2 + (0.72686e-3 + (0.36843e-5 + (0.18071e-7 + (0.85496e-10 + (0.38852e-12 + 0.16868e-14 * t) * t) * t) * t) * t) * t;
    case 2:
      t = 2 * x - 5;
      return 0.36165e-2 + (0.74182e-3 + (0.37948e-5 + (0.18771e-7 + (0.89484e-10 + (0.40935e-12 + 0.17872e-14 * t) * t) * t) * t) * t) * t;
    case 3:
      t = 2 * x - 7;
      return 0.51154e-2 + (0.75722e-3 + (0.39096e-5 + (0.19504e-7 + (0.93687e-10 + (0.43143e-12 + 0.18939e-14 * t) * t) * t) * t) * t) * t;
    case 4:
      t = 2 * x - 9;
      return 0.66457e-2 + (0.77310e-3 + (0.40289e-5 + (0.20271e-7 + (0.98117e-10 + (0.45484e-12 + 0.20076e-14 * t) * t) * t) * t) * t) * t;
    case 5:
      t = 2 * x - 11;
      return 0.82082e-2 + (0.78946e-3 + (0.41529e-5 + (0.21074e-7 + (0.10278e-9 + (0.47965e-12 + 0.21285e-14 * t) * t) * t) * t) * t) * t;
    case 6:
      t = 2 * x - 13;
      return 0.98039e-2 + (0.80633e-3 + (0.42819e-5 + (0.21916e-7 + (0.10771e-9 + (0.50595e-12 + 0.22573e-14 * t) * t) * t) * t) * t) * t;
    case 7:
      t = 2 * x - 15;
      return 0.11433e-1 + (0.82372e-3 + (0.44160e-5 + (0.22798e-7 + (0.11291e-9 + (0.53386e-12 + 0.23944e-14 * t) * t) * t) * t) * t) * t;
    case 8:
      t = 2 * x - 17;
      return 0.13099e-1 + (0.84167e-3 + (0.45555e-5 + (0.23723e-7 + (0.11839e-9 + (0.56346e-12 + 0.25403e-14 * t) * t) * t) * t) * t) * t;
    case 9:
      t = 2 * x - 19;
      return 0.14800e-1 + (0.86018e-3 + (0.47008e-5 + (0.24694e-7 + (0.12418e-9 + (0.59486e-12 + 0.26957e-14 * t) * t) * t) * t) * t) * t;
    case 10:
      t = 2 * x - 21;
      return 0.16540e-1 + (0.87928e-3 + (0.48520e-5 + (0.25711e-7 + (0.13030e-9 + (0.62820e-12 + 0.28612e-14 * t) * t) * t) * t) * t) * t;
    case 11:
      t = 2 * x - 23;
      return 0.18318e-1 + (0.89900e-3 + (0.50094e-5 + (0.26779e-7 + (0.13675e-9 + (0.66358e-12 + 0.30375e-14 * t) * t) * t) * t) * t) * t;
    case 12:
      t = 2 * x - 25;
      return 0.20136e-1 + (0.91936e-3 + (0.51734e-5 + (0.27900e-7 + (0.14357e-9 + (0.70114e-12 + 0.32252e-14 * t) * t) * t) * t) * t) * t;
    case 13:
      t = 2 * x - 27;
      return 0.21996e-1 + (0.94040e-3 + (0.53443e-5 + (0.29078e-7 + (0.15078e-9 + (0.74103e-12 + 0.34251e-14 * t) * t) * t) * t) * t) * t;
    case 14:
      t = 2 * x - 29;
      return 0.23898e-1 + (0.96213e-3 + (0.55225e-5 + (0.30314e-7 + (0.15840e-9 + (0.78340e-12 + 0.36381e-14 * t) * t) * t) * t) * t) * t;
    case 15:
      t = 2 * x - 31;
      return 0.25845e-1 + (0.98459e-3 + (0.57082e-5 + (0.31613e-7 + (0.16646e-9 + (0.82840e-12 + 0.38649e-14 * t) * t) * t) * t) * t) * t;
    case 16:
      t = 2 * x - 33;
      return 0.27837e-1 + (0.10078e-2 + (0.59020e-5 + (0.32979e-7 + (0.17498e-9 + (0.87622e-12 + 0.41066e-14 * t) * t) * t) * t) * t) * t;
    case 17:
      t = 2 * x - 35;
      return 0.29877e-1 + (0.10318e-2 + (0.61041e-5 + (0.34414e-7 + (0.18399e-9 + (0.92703e-12 + 0.43639e-14 * t) * t) * t) * t) * t) * t;
    case 18:
      t = 2 * x - 37;
      return 0.31965e-1 + (0.10566e-2 + (0.63151e-5 + (0.35924e-7 + (0.19353e-9 + (0.98102e-12 + 0.46381e-14 * t) * t) * t) * t) * t) * t;
    case 19:
      t = 2 * x - 39;
      return 0.34104e-1 + (0.10823e-2 + (0.65354e-5 + (0.37512e-7 + (0.20362e-9 + (0.10384e-11 + 0.49300e-14 * t) * t) * t) * t) * t) * t;
    case 20:
      t = 2 * x - 41;
      return 0.36295e-1 + (0.11089e-2 + (0.67654e-5 + (0.39184e-7 + (0.21431e-9 + (0.10994e-11 + 0.52409e-14 * t) * t) * t) * t) * t) * t;
    case 21:
      t = 2 * x - 43;
      return 0.38540e-1 + (0.11364e-2 + (0.70058e-5 + (0.40943e-7 + (0.22563e-9 + (0.11642e-11 + 0.55721e-14 * t) * t) * t) * t) * t) * t;
    case 22:
      t = 2 * x - 45;
      return 0.40842e-1 + (0.11650e-2 + (0.72569e-5 + (0.42796e-7 + (0.23761e-9 + (0.12332e-11 + 0.59246e-14 * t) * t) * t) * t) * t) * t;
    case 23:
      t = 2 * x - 47;
      return 0.43201e-1 + (0.11945e-2 + (0.75195e-5 + (0.44747e-7 + (0.25030e-9 + (0.13065e-11 + 0.63000e-14 * t) * t) * t) * t) * t) * t;
    case 24:
      t = 2 * x - 49;
      return 0.45621e-1 + (0.12251e-2 + (0.77941e-5 + (0.46803e-7 + (0.26375e-9 + (0.13845e-11 + 0.66996e-14 * t) * t) * t) * t) * t) * t;
    case 25:
      t = 2 * x - 51;
      return 0.48103e-1 + (0.12569e-2 + (0.80814e-5 + (0.48969e-7 + (0.27801e-9 + (0.14674e-11 + 0.71249e-14 * t) * t) * t) * t) * t) * t;
    case 26:
      t = 2 * x - 59;
      return 0.58702e-1 + (0.13962e-2 + (0.93714e-5 + (0.58882e-7 + (0.34414e-9 + (0.18552e-11 + 0.91160e-14 * t) * t) * t) * t) * t) * t;
    case 30:
      t = 2 * x - 79;
      return 0.90908e-1 + (0.18544e-2 + (0.13903e-4 + (0.95549e-7 + (0.59752e-9 + (0.33656e-11 + 0.16815e-13 * t) * t) * t) * t) * t) * t;
    case 40:
      t = 2 * x - 99;
      return 0.13443e0 + (0.25474e-2 + (0.21385e-4 + (0.15996e-6 + (0.10585e-8 + (0.61258e-11 + 0.30412e-13 * t) * t) * t) * t) * t) * t;
    case 50:
      t = 2 * x - 119;
      return 0.19540e0 + (0.36342e-2 + (0.34096e-4 + (0.27479e-6 + (0.18934e-8 + (0.11021e-10 + 0.52931e-13 * t) * t) * t) * t) * t) * t;
    case 60:
      t = 2 * x - 121;
      return 0.20281e0 + (0.37739e-2 + (0.35791e-4 + (0.29038e-6 + (0.20068e-8 + (0.11673e-10 + 0.55790e-13 * t) * t) * t) * t) * t) * t;
    case 61:
      t = 2 * x - 123;
      return 0.21050e0 + (0.39206e-2 + (0.37582e-4 + (0.30691e-6 + (0.21270e-8 + (0.12361e-10 + 0.58770e-13 * t) * t) * t) * t) * t) * t;
    case 62:
      t = 2 * x - 125;
      return 0.21849e0 + (0.40747e-2 + (0.39476e-4 + (0.32443e-6 + (0.22542e-8 + (0.13084e-10 + 0.61873e-13 * t) * t) * t) * t) * t) * t;
    case 63:
      t = 2 * x - 127;
      return 0.22680e0 + (0.42366e-2 + (0.41477e-4 + (0.34300e-6 + (0.23888e-8 + (0.13846e-10 + 0.65100e-13 * t) * t) * t) * t) * t) * t;
    case 64:
      t = 2 * x - 129;
      return 0.23545e0 + (0.44067e-2 + (0.43594e-4 + (0.36268e-6 + (0.25312e-8 + (0.14647e-10 + 0.68453e-13 * t) * t) * t) * t) * t) * t;
    case 65:
      t = 2 * x - 131;
      return 0.24444e0 + (0.45855e-2 + (0.45832e-4 + (0.38352e-6 + (0.26819e-8 + (0.15489e-10 + 0.71933e-13 * t) * t) * t) * t) * t) * t;
    case 66:
      t = 2 * x - 133;
      return 0.25379e0 + (0.47735e-2 + (0.48199e-4 + (0.40561e-6 + (0.28411e-8 + (0.16374e-10 + 0.75541e-13 * t) * t) * t) * t) * t) * t;
    case 67:
      t = 2 * x - 135;
      return 0.26354e0 + (0.49713e-2 + (0.50702e-4 + (0.42901e-6 + (0.30095e-8 + (0.17303e-10 + 0.79278e-13 * t) * t) * t) * t) * t) * t;
    case 68:
      t = 2 * x - 137;
      return 0.27369e0 + (0.51793e-2 + (0.53350e-4 + (0.45379e-6 + (0.31874e-8 + (0.18277e-10 + 0.83144e-13 * t) * t) * t) * t) * t) * t;
    case 69:
      t = 2 * x - 139;
      return 0.28426e0 + (0.53983e-2 + (0.56150e-4 + (0.48003e-6 + (0.33752e-8 + (0.19299e-10 + 0.87139e-13 * t) * t) * t) * t) * t) * t;
    case 70:
      t = 2 * x - 141;
      return 0.29529e0 + (0.56288e-2 + (0.59113e-4 + (0.50782e-6 + (0.35735e-8 + (0.20369e-10 + 0.91262e-13 * t) * t) * t) * t) * t) * t;
    case 71:
      t = 2 * x - 143;
      return 0.30679e0 + (0.58714e-2 + (0.62248e-4 + (0.53724e-6 + (0.37827e-8 + (0.21490e-10 + 0.95513e-13 * t) * t) * t) * t) * t) * t;
    case 72:
      t = 2 * x - 145;
      return 0.31878e0 + (0.61270e-2 + (0.65564e-4 + (0.56837e-6 + (0.40035e-8 + (0.22662e-10 + 0.99891e-13 * t) * t) * t) * t) * t) * t;
    case 73:
      t = 2 * x - 147;
      return 0.33130e0 + (0.63962e-2 + (0.69072e-4 + (0.60133e-6 + (0.42362e-8 + (0.23888e-10 + 0.10439e-12 * t) * t) * t) * t) * t) * t;
    case 74:
      t = 2 * x - 149;
      return 0.34438e0 + (0.66798e-2 + (0.72783e-4 + (0.63619e-6 + (0.44814e-8 + (0.25168e-10 + 0.10901e-12 * t) * t) * t) * t) * t) * t;
    case 75:
      t = 2 * x - 151;
      return 0.35803e0 + (0.69787e-2 + (0.76710e-4 + (0.67306e-6 + (0.47397e-8 + (0.26505e-10 + 0.11376e-12 * t) * t) * t) * t) * t) * t;
    case 76:
      t = 2 * x - 153;
      return 0.37230e0 + (0.72938e-2 + (0.80864e-4 + (0.71206e-6 + (0.50117e-8 + (0.27899e-10 + 0.11862e-12 * t) * t) * t) * t) * t) * t;
    case 77:
      t = 2 * x - 155;
      return 0.38722e0 + (0.76260e-2 + (0.85259e-4 + (0.75329e-6 + (0.52979e-8 + (0.29352e-10 + 0.12360e-12 * t) * t) * t) * t) * t) * t;
    case 78:
      t = 2 * x - 157;
      return 0.40282e0 + (0.79762e-2 + (0.89909e-4 + (0.79687e-6 + (0.55989e-8 + (0.30866e-10 + 0.12868e-12 * t) * t) * t) * t) * t) * t;
    case 79:
      t = 2 * x - 159;
      return 0.41914e0 + (0.83456e-2 + (0.94827e-4 + (0.84291e-6 + (0.59154e-8 + (0.32441e-10 + 0.13387e-12 * t) * t) * t) * t) * t) * t;
    case 80:
      t = 2 * x - 161;
      return 0.43621e0 + (0.87352e-2 + (0.10002e-3 + (0.89156e-6 + (0.62480e-8 + (0.34079e-10 + 0.13917e-12 * t) * t) * t) * t) * t) * t;
    case 81:
      t = 2 * x - 163;
      return 0.45409e0 + (0.91463e-2 + (0.10553e-3 + (0.94293e-6 + (0.65972e-8 + (0.35782e-10 + 0.14455e-12 * t) * t) * t) * t) * t) * t;
    case 82:
      t = 2 * x - 165;
      return 0.47282e0 + (0.95799e-2 + (0.11135e-3 + (0.99716e-6 + (0.69638e-8 + (0.37549e-10 + 0.15003e-12 * t) * t) * t) * t) * t) * t;
    case 83:
      t = 2 * x - 167;
      return 0.49243e0 + (0.10037e-1 + (0.11750e-3 + (0.10544e-5 + (0.73484e-8 + (0.39383e-10 + 0.15559e-12 * t) * t) * t) * t) * t) * t;
    case 84:
      t = 2 * x - 169;
      return 0.51298e0 + (0.10520e-1 + (0.12400e-3 + (0.11147e-5 + (0.77517e-8 + (0.41283e-10 + 0.16122e-12 * t) * t) * t) * t) * t) * t;
    case 85:
      t = 2 * x - 171;
      return 0.53453e0 + (0.11030e-1 + (0.13088e-3 + (0.11784e-5 + (0.81743e-8 + (0.43252e-10 + 0.16692e-12 * t) * t) * t) * t) * t) * t;
    case 86:
      t = 2 * x - 173;
      return 0.55712e0 + (0.11568e-1 + (0.13815e-3 + (0.12456e-5 + (0.86169e-8 + (0.45290e-10 + 0.17268e-12 * t) * t) * t) * t) * t) * t;
    case 87:
      t = 2 * x - 175;
      return 0.58082e0 + (0.12135e-1 + (0.14584e-3 + (0.13164e-5 + (0.90803e-8 + (0.47397e-10 + 0.17850e-12 * t) * t) * t) * t) * t) * t;
    case 88:
      t = 2 * x - 177;
      return 0.60569e0 + (0.12735e-1 + (0.15396e-3 + (0.13909e-5 + (0.95651e-8 + (0.49574e-10 + 0.18435e-12 * t) * t) * t) * t) * t) * t;
    case 89:
      t = 2 * x - 179;
      return 0.63178e0 + (0.13368e-1 + (0.16254e-3 + (0.14695e-5 + (0.10072e-7 + (0.51822e-10 + 0.19025e-12 * t) * t) * t) * t) * t) * t;
    case 90:
      t = 2 * x - 181;
      return 0.65918e0 + (0.14036e-1 + (0.17160e-3 + (0.15521e-5 + (0.10601e-7 + (0.54140e-10 + 0.19616e-12 * t) * t) * t) * t) * t) * t;
    case 91:
      t = 2 * x - 183;
      return 0.68795e0 + (0.14741e-1 + (0.18117e-3 + (0.16392e-5 + (0.11155e-7 + (0.56530e-10 + 0.20209e-12 * t) * t) * t) * t) * t) * t;
    case 92:
      t = 2 * x - 185;
      return 0.71818e0 + (0.15486e-1 + (0.19128e-3 + (0.17307e-5 + (0.11732e-7 + (0.58991e-10 + 0.20803e-12 * t) * t) * t) * t) * t) * t;
    case 93:
      t = 2 * x - 187;
      return 0.74993e0 + (0.16272e-1 + (0.20195e-3 + (0.18269e-5 + (0.12335e-7 + (0.61523e-10 + 0.21395e-12 * t) * t) * t) * t) * t) * t;
    }
  return 1.0;
}

int
main ()
{
#ifdef __s390x__
  {
    register unsigned long r5 __asm ("r5");
    r5 = 0xdeadbeefUL;
    asm volatile ("":"+r" (r5));
  }
#endif
  double d = foo (78.4);
  if (d < 0.38 || d > 0.42)
    __builtin_abort ();
  return 0;
}
