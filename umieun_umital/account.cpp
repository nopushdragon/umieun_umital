#include "account.h"

Account account;

Account::Account() {
	easy_time = 0.0f;
	normal_time = 0.0f;
	hard_time = 0.0f;

	// 파일 읽기 시도
	ifstream inFile("account.txt");
	
	if (inFile.is_open()) {
		// 파일이 존재하면 읽기
		while (inFile) {
			string level;
			float time;
			inFile >> level >> time;
			if (level == "easy") {
				easy_time = time;
			}
			else if (level == "normal") {
				normal_time = time;
			}
			else if (level == "hard") {
				hard_time = time;
			}
		}
		inFile.close();
		cout << "[Account] 기록 파일 로드 완료" << endl;
	}
	else {
		// 파일이 없으면 새로 생성
		cout << "[Account] 기록 파일이 없어 새로 생성합니다." << endl;
		ofstream outFile("account.txt");
		if (outFile.is_open()) {
			outFile << "easy 0" << endl;
			outFile << "normal 0" << endl;
			outFile << "hard 0" << endl;
			outFile.close();
			cout << "[Account] 기록 파일 생성 완료" << endl;
		}
		else {
			cerr << "[Account] ERROR: 파일 생성 실패!" << endl;
		}
	}

	scores[0] = easy_time;
	scores[1] = normal_time;
	scores[2] = hard_time;
}

string Account::best_score(int level) {
	float time = 0.0f;
	
	if (level == 0) {
		time = easy_time;
	}
	else if (level == 1) {
		time = normal_time;
	}
	else if (level == 2) {
		time = hard_time;
	}
	
	if (time == 0.0f) {
		return "NONE";
	}
	
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "%.2f", time);
	return string(buffer);
}

void Account::update_score(int level, float time) {
	if (level == 0) {
		if (easy_time == 0.0f || time < easy_time) {
			easy_time = time;
		}
	}
	else if (level == 1) {
		if (normal_time == 0.0f || time < normal_time) {
			normal_time = time;
		}
	}
	else if (level == 2) {
		if (hard_time == 0.0f || time < hard_time) {
			hard_time = time;
		}
	}
	
	// 파일에 저장
	ofstream outFile("account.txt");
	if (outFile.is_open()) {
		outFile << "easy " << easy_time << endl;
		outFile << "normal " << normal_time << endl;
		outFile << "hard " << hard_time << endl;
		outFile.close();
		cout << "account에 기록 저장 완료" << endl;
	}
	else {
		cerr << "account에 파일 저장 실패!" << endl;
	}
}