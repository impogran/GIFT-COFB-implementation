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

//Truncating string into <length> first bits.
string stringTruncate(string x, int length) {
	char result[128]{};
	if (length > x.length()) {
		return "Given truncating length is bigger than original string's length.";
	}
	for (int i = 0; i < length; i++) {
		result[i] = x[i];
	}
	return result;
}

// Check if string's length is the same as the block size
bool isComplete(string x) {
	if (x.length() == blockSize) return true;
	return false;
}

// Check for repetitions of nonces (they should be non-repetetive)
bool checkRepeated(const char *x, const char *y) {
	if (!strcmp(x, y)) return true;
	return false;
}

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
	cout << "Rows in the 2d vector: " << init.size() <<
		endl << "Collumns in the 1st row: " << init[0].size() << endl;

	for (int i = rows - 1; i >= 0; i--) {
		cout << i << endl;
		for (int j = cols - 1; j >= 0; j--) {
			cout << j << " ";
			init[i][j] = key[keyIndex];
			keyIndex++;

		}
	}

	return init;
}

int main() {

	// Inputs
	vector<unsigned char> encryptionKey, nonce, message = readFile("Message.txt");

	//Processed data
	vector<vector<unsigned char>> cipherState;
	vector<vector<unsigned char>> cipherKey;

	// Outputs
	vector<unsigned char> cipherText, tag;

	//Initialization puts 128-bit plaintext into 4 segments, top-to-bottom, right-to-left
	cipherState = initializeVector(4, 32, message);

	//Key is initialized into 4 segments right-to-left, bottom-to-top
	cipherKey = initializeKey(4, 32, encryptionKey);
	/*
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 32; j++) {
			cout<<cipherKey[i][j];
		}
		cout << endl;
	}
	*/

	// Each encryption takes 40 rounds with 3 steps - SubCells, PermBits, and AddRoundKey

	return 0;
}