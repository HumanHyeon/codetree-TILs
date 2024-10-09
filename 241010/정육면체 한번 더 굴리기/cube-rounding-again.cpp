#include <bits/stdc++.h>
using namespace std;
using pii = pair<int, int>;

#define DEBUG 0
#define TOP 0
#define BOTTOM 2
enum DIRECTION { UP, RIGHT, DOWN, LEFT };

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };

class Solution {
public:
	int answer;

	int N;
	int M;
	vector<vector<int> > MAP;
	vector<int> DICE;
	pii POS;
	int FLAG;

	vector<vector<int> > SEC;
	vector<int> secCnt;
	int secId;
	
	Solution() : answer(0) {}

	bool printArr(vector<vector<int> >& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < T.size(); ++y) {
			for (int x = 0; x < T[0].size(); ++x)
				printf("%3d ", T[y][x]);
			putchar('\n');
		}
		putchar('\n');

		return (true);
	}

	inline bool isValid(int y, int x) {
		return (!(y < 0 || x < 0 || y >= N || x >= N));
	}

	void setDice(int flag) {
		int tmp;

		if (flag == UP) {
			tmp = DICE[0];
			DICE[0] = DICE[1];
			DICE[1] = DICE[2];
			DICE[2] = DICE[3];
			DICE[3] = tmp;
		}	
		else if (flag == RIGHT) {
			tmp = DICE[0];
			DICE[0] = DICE[4];
			DICE[4] = DICE[2];
			DICE[2] = DICE[5];
			DICE[5] = tmp;
		}
		else if (flag == DOWN) {
			tmp = DICE[0];
			DICE[0] = DICE[3];
			DICE[3] = DICE[2];
			DICE[2] = DICE[1];
			DICE[1] = tmp;
		}
		else {
			tmp = DICE[0];
			DICE[0] = DICE[5];
			DICE[5] = DICE[2];
			DICE[2] = DICE[4];
			DICE[4] = tmp;
		}
	}

	void setSection() {
		queue<pii> q;

		for (int y = 0; y < N; ++y)
			for (int x = 0; x < N; ++x)
				if (SEC[y][x] == 0) {
					int id = MAP[y][x]; 
					int cnt = 1;

					q.push(make_pair(y, x));
					SEC[y][x] = ++secId;
					while (!q.empty()) {
						int ty = q.front().first;
						int tx = q.front().second;
						q.pop();

						for (int flag = 0; flag < 4; ++flag) {
							int ny = ty + dy[flag];
							int nx = tx + dx[flag];

							if (!isValid(ny, nx))	continue;
							if (SEC[ny][nx] != 0 || MAP[ny][nx] != id)	continue;

							q.push(make_pair(ny, nx));
							SEC[ny][nx] = secId;
							++cnt;
						} 
					}
					secCnt[secId] = cnt;
				}
	}

	inline int getScore(int y, int x) {
		return (MAP[y][x] * secCnt[SEC[y][x]]);
	}

	void solution() {
		DEBUG && printArr(MAP, "MAP 초기 상태");
		setSection();
		DEBUG && printArr(SEC, "section 상태");
		if (DEBUG) {
			cout << "section 정보: ";
			for (int i = 1; i <= secId; ++i)
				cout << "(id: " << i << " 갯수: " << secCnt[i] << " ) ";
			putchar('\n');
		}
	
		setDice(RIGHT);
		POS.second = 1;
		for (int turn = 0; turn < M; ++turn) {
			int score;
			int bot;
			
			score = getScore(POS.first, POS.second);
			answer += score;
			DEBUG && cout << turn << " 턴에 획득한 점수: " << score << "  총 점수: " << answer << "\n";

			bot = DICE[BOTTOM];
			if (bot > MAP[POS.first][POS.second])	++FLAG;
			else if (bot < MAP[POS.first][POS.second])	--FLAG;
			FLAG += 4;
			FLAG %= 4;

			if (!isValid(POS.first + dy[FLAG], POS.second + dx[FLAG])) {
				FLAG += 2;
				FLAG %= 4;
			}

			setDice(FLAG);
			POS.first += dy[FLAG];
			POS.second += dx[FLAG];
		}
	}

	void input() {
		cin >> N >> M;

		MAP.resize(N, vector<int>(N));
		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)	cin >> MAP[y][x];

		SEC.resize(N, vector<int>(N, 0));
		secCnt.resize(N * N + 2, 0);
		secId = 0;

		POS = make_pair(0, 0);
		FLAG = RIGHT;

		DICE.resize(6);
		DICE[0] = 1;
		DICE[1] = 2;
		DICE[2] = 6;
		DICE[3] = 5;
		DICE[4] = 4;
		DICE[5] = 3;
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	Solution s;
	s.input();
	s.solution();
	cout << s.answer;
}