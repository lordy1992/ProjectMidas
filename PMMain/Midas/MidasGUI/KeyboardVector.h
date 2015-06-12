#include <vector>
#include <Windows.h>

class KeyboardVector {
public:
	void clear();

	void inputCharDown(char c);

	void inputCharUp(char c);

	void inputCharDownUp(char c);

	void inputVKDown(unsigned int vk);

	void inputVKUp(unsigned int vk);

	void inputVKDownUp(unsigned int vk);

	const std::vector<KEYBDINPUT> peakVector() { return kiVector; }

private:
	std::vector<KEYBDINPUT> kiVector;
};