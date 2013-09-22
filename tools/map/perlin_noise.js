// perlin_noise(x, y, z)

(function() {

const p = [151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,
	37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,
	33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,
	231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,
	1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,
	173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,
	16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
	172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,
	193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,
	157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
	128,195,78,66,215,61,156,180,151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,
	103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
	35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,
	166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
	65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,
	100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
	59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,
	155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,
	181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,
	72,243,141,128,195,78,66,215,61,156,180];

var iOctaves = 1;
var fPersistence = .5;

var aOctFreq; // frequency per octave
var aOctPers; // persistence per octave
var fPersMax; // 1 / max persistence

var iSeed = 1323;

var iXoffset;
var iYoffset;
var iZoffset;

const baseFactor = 1 / 64;

var initialized = false;

function init()
{
	seedOffset();
	octFreqPers();
	initialized = true;
}

function octFreqPers()
{
	var fFreq, fPers;
	aOctFreq = [];
	aOctPers = [];
	fPersMax = 0;

	for (var i = 0; i < iOctaves; i++)
	{
		fFreq = Math.pow(2, i);
		fPers = Math.pow(fPersistence, i);
		fPersMax += fPers;
		aOctFreq.push(fFreq);
		aOctPers.push(fPers);
	}

	fPersMax = 1 / fPersMax;
}

function seedOffset()
{
	iXoffset = iSeed = (iSeed * 16807) % 2147483647;
	iYoffset = iSeed = (iSeed * 16807) % 2147483647;
	iZoffset = iSeed = (iSeed * 16807) % 2147483647;
}

function noise(x, y, z)
{
	x = x || 0;
	y = y || 0;
	z = z || 0;

	if (!initialized)
		init();

	var s = 0;
	var fFreq, fPers, x, y, z;
	var xf, yf, zf, u, v, w;
	var x1, y1, z1;
	var X, Y, Z, A, B, AA, AB, BA, BB, hash;
	var g1, g2, g3, g4, g5, g6, g7, g8;

	x += iXoffset;
	y += iYoffset;
	z += iZoffset;

	var int = Math.round;

	for (var i = 0; i < iOctaves; i++)
	{
		fFreq = aOctFreq[i];
		fPers = aOctPers[i];

		x = x * fFreq;
		y = y * fFreq;
		z = z * fFreq;

		xf = Math.floor(x);
		yf = Math.floor(y);
		zf = Math.floor(z);

		X = xf & 255;
		Y = yf & 255;
		Z = zf & 255;

		x -= xf;
		y -= yf;
		z -= zf;

		u = x * x * x * (x * (x * 6 - 15) + 10);
		v = y * y * y * (y * (y * 6 - 15) + 10);
		w = z * z * z * (z * (z * 6 - 15) + 10);

		A  = int(p[X]) + Y;
		AA = int(p[A]) + Z;
		AB = int(p[int(A + 1)]) + Z;
		B  = int(p[int(X + 1)]) + Y;
		BA = int(p[B]) + Z;
		BB = int(p[int(B + 1)]) + Z;

		x1 = x - 1;
		y1 = y - 1;
		z1 = z - 1;

		hash = int(p[int(BB + 1)]) & 15;
		g1 = ((hash&1) == 0 ? (hash<8 ? x1 : y1) : (hash<8 ? -x1 : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x1 : z1) : hash<4 ? -y1 : (hash==14 ? -x1 : -z1));

		hash = int(p[int(AB + 1)]) & 15;
		g2 = ((hash&1) == 0 ? (hash<8 ? x  : y1) : (hash<8 ? -x  : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x  : z1) : hash<4 ? -y1 : (hash==14 ? -x : -z1));

		hash = int(p[int(BA + 1)]) & 15;
		g3 = ((hash&1) == 0 ? (hash<8 ? x1 : y ) : (hash<8 ? -x1 : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x1 : z1) : hash<4 ? -y  : (hash==14 ? -x1 : -z1));

		hash = int(p[int(AA + 1)]) & 15;
		g4 = ((hash&1) == 0 ? (hash<8 ? x  : y ) : (hash<8 ? -x  : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x  : z1) : hash<4 ? -y  : (hash==14 ? -x  : -z1));

		hash = int(p[BB]) & 15;
		g5 = ((hash&1) == 0 ? (hash<8 ? x1 : y1) : (hash<8 ? -x1 : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x1 : z) : hash<4 ? -y1 : (hash==14 ? -x1 : -z));

		hash = int(p[AB]) & 15;
		g6 = ((hash&1) == 0 ? (hash<8 ? x  : y1) : (hash<8 ? -x  : -y1)) + ((hash&2) == 0 ? hash<4 ? y1 : (hash==12 ? x  : z) : hash<4 ? -y1 : (hash==14 ? -x  : -z));

		hash = int(p[BA]) & 15;
		g7 = ((hash&1) == 0 ? (hash<8 ? x1 : y ) : (hash<8 ? -x1 : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x1 : z) : hash<4 ? -y  : (hash==14 ? -x1 : -z));

		hash = int(p[AA]) & 15;
		g8 = ((hash&1) == 0 ? (hash<8 ? x  : y ) : (hash<8 ? -x  : -y )) + ((hash&2) == 0 ? hash<4 ? y  : (hash==12 ? x  : z) : hash<4 ? -y  : (hash==14 ? -x  : -z));

		g2 += u * (g1 - g2);
		g4 += u * (g3 - g4);
		g6 += u * (g5 - g6);
		g8 += u * (g7 - g8);

		g4 += v * (g2 - g4);
		g8 += v * (g6 - g8);

		s += (g8 + w * (g4 - g8)) * fPers;
	}

	return (s * fPersMax + 1) * .5;
}

perlin_noise = noise;

})();
