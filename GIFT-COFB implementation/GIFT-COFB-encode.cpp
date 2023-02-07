#include <iostream>
#include <string>
#include <bitset>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
using namespace std;

int blockSize = 128;
int tagLength = 128;
//E_k (underlying block cipher) is GIFT-128

vector<unsigned char> readFile(const char* filename)
{
	ifstream file(filename, ios::binary);
	file.unsetf(ios::skipws);
	streampos fileSize;

	file.seekg(0, ios::end);
	fileSize = file.tellg();
	file.seekg(0, ios::beg);

	std::vector<unsigned char> vec;
	vec.reserve(fileSize);

	vec.insert(vec.begin(),
		std::istream_iterator<unsigned char>(file),
		std::istream_iterator<unsigned char>());

	return vec;
}

vector<vector<unsigned char>> initializeVector(int rows, int cols, vector<unsigned char> message) {
	int messageIndex = 0;
	vector<vector<unsigned char>> init(rows, vector<unsigned char>(cols));

	for (int i = cols - 1; i >= 0; i--) {
		for (int j = 0; j < rows; j++) {
			init[j][i] = message[messageIndex];
			messageIndex++;
		}
	}

	return init;
}

vector<vector<unsigned char>> initializeKey(int rows, int cols, vector<unsigned char> key) {
	int keyIndex = 0;
	vector<vector<unsigned char>> init(rows, vector<unsigned char>(cols));

	for (int i = rows - 1; i >= 0; i--) {
		for (int j = cols - 1; j >= 0; j--) {
			init[i][j] = key[keyIndex];
			keyIndex++;

		}
	}

	return init;
}

vector<unsigned char> subCells(vector<unsigned char> s0, vector<unsigned char> s1, vector<unsigned char> s2, vector<unsigned char> s3) {
	/*
	Update the cipher state with the following instructions:
	S1 ← S1 ⊕ (S0 & S2)
	S0 ← S0 ⊕ (S1 & S3)
	S2 ← S2 ⊕ (S0 | S1)
	S3 ← S3 ⊕ S2
	S1 ← S1 ⊕ S3
	S3 ← ∼ S3
	S2 ← S2 ⊕ (S0 & S1)
	{S0, S1, S2, S3} ← {S3, S1, S2, S0}
	*/

	for (int i = 0; i < 32; i++) { s1[i] = s1[i] ^ (s0[i] & s2[i]); }
	for (int i = 0; i < 32; i++) { s0[i] = s0[i] ^ (s1[i] & s3[i]); }
	for (int i = 0; i < 32; i++) { s2[i] = s2[i] ^ (s0[i] | s1[i]); }
	for (int i = 0; i < 32; i++) { s3[i] = s3[i] ^ s2[i]; }
	for (int i = 0; i < 32; i++) { s1[i] = s1[i] ^ s3[i]; }
	for (int i = 0; i < 32; i++) { s3[i] == '0' ? s3[i] = '1' : s3[i] = '0'; } //negation doesn't work normally as those are letters
	for (int i = 0; i < 32; i++) { s2[i] = s2[i] ^ (s0[i] & s1[i]); }
	
	vector<unsigned char> tmp0 = s0;
	vector<unsigned char> tmp3 = s3;

	s0 = tmp3; s3 = tmp0;

	return s0, s1, s2, s3;
}

int main() {

	// Inputs
	vector<unsigned char> encryptionKey = readFile("Key.txt"), nonce, message = readFile("Message.txt");

	//Processed data
	vector<vector<unsigned char>> cipherState;
	vector<unsigned char> s0, s1, s2, s3;
	vector<vector<unsigned char>> cipherKey;
	vector<unsigned char> l0, l1, l2, l3, ks0, ks1, ks2, ks3, ks4, ks5, ks6, ks7;

	// Outputs
	vector<unsigned char> cipherText, tag;

	//Initialization puts 128-bit plaintext into 4 segments of 32-bit words, top-to-bottom, right-to-left
	cipherState = initializeVector(4, 32, message);
	s0 = cipherState[0];
	s1 = cipherState[1];
	s2 = cipherState[2];
	s3 = cipherState[3];
	
	//Key is initialized into 8 segments of 16-bit words, right-to-left, bottom-to-top
	cipherKey = initializeKey(4, 32, encryptionKey);
	l0 = cipherKey[0]; l1 = cipherKey[1]; l2 = cipherKey[2]; l3 = cipherKey[3];
	ks0 = { l0.begin(), l0.end() - 16 }; ks1 = { l0.begin() + 16, l0.end() };
	ks2 = { l1.begin(), l1.end() - 16 }; ks3 = { l1.begin() + 16, l1.end() };
	ks4 = { l2.begin(), l2.end() - 16 }; ks5 = { l2.begin() + 16, l2.end() };
	ks6 = { l3.begin(), l3.end() - 16 }; ks7 = { l3.begin() + 16, l3.end() };

	// Each encryption takes 40 rounds with 3 steps - SubCells, PermBits, and AddRoundKey
	//SubCells (subCells function)
	s0, s1, s2, s3 = subCells(s0, s1, s2, s3);
	//PermBits (permBits function)

	
	return 0;
}