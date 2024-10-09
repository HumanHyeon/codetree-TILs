#include <bits/stdc++.h>
using namespace std;
using pii = pair<int, int>;

#define DEBUG 0

const int dy[] = { 0, -1, -1, -1, 0, 1, 1, 1 };
const int dx[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
const int ddy[] = { -1, -1, 1, 1 };
const int ddx[] = { -1, 1, -1, 1 };

class Solution {
public:
	int N, M;
	vector<vector<int> > MAP;
	vector<pii> CMD;
	int answer;

	deque<pii> POS;
	vector<vector<int> > TMP;

	Solution() : answer(0) {}

	bool printArr(vector<vector<int> >& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x)
				printf("%3d ", T[y][x]);
			putchar('\n');
		}		
		putchar('\n');

		return (true);
	}

	inline bool isValid(int y, int x) {
		return(!(y < 0 || x < 0 || y >= N || x >= N));
	}

	/**
	 * TODO: POS에 있는 영양제들을 cmd에 맞춰 이동. 이동한 영양제는 다시 POS에 넣으며, TMP에 이동한 위치 갱신 
	 */
	void movePos(const pii& cmd) {
		int len = POS.size();
		int flag = cmd.first - 1;
		int amount = cmd.second;

		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)	TMP[y][x] = 0;

		while (len--) {
			int y = POS.front().first;
			int x = POS.front().second;
			POS.pop_front();

			y += (dy[flag] * amount);
			x += (dx[flag] * amount);
			y += N;
			x += N;
			y %= N;	// [1-2]
			x %= N;

			TMP[y][x] = 1;
			POS.push_back(make_pair(y, x));
		}
	}

	/**
	 * TODO: 영양제가 놓여 있는 칸들에 대해 높이 1증가
	 */
	inline void injectPos() {
		int len = POS.size();
		for (int i = 0; i < len; ++i)
			++MAP[POS.at(i).first][POS.at(i).second]; // [2]
	}

	/**
	 * TODO: 4개의 대각선에 있는 칸들 확인하여 높이가 1이상인 개수만큼 높이 증가
	 */
	void calcDiag() {
		while (!POS.empty()) {
			int y = POS.front().first;
			int x = POS.front().second;
			int cnt = 0;
			POS.pop_front();
		
			for (int flag = 0; flag < 4; ++flag) {
				int ny = y + ddy[flag];
				int nx = x + ddx[flag];

				if (!isValid(ny, nx))	continue;
				if (MAP[ny][nx] == 0)	continue;

				++cnt;
			}
			//cout << "y: " << y << " x: " << x << "  cnt: " << cnt << endl;
			MAP[y][x] += cnt;
			//한 번에 업데이트 해야하는지 확인 필요 -> 현재는 필요하지 않음
		}
	}

	void prunMAP() {
		for (int y = 0; y < N; ++y) 
			for (int x = 0; x < N; ++x) {
				if (TMP[y][x] == 1)	continue;
				if (MAP[y][x] >= 2) {
					MAP[y][x] -= 2;
					POS.push_back(make_pair(y, x));
				}
			}
	}

	void solution() {
		/**
		 * 1. 특수 영양제를 이동 규칙에 따라 이동. 이동 규칙은 CMD에 저장되어 있음
		 * 	1-1. 초기 특수 영양제는 좌하단 4개 칸에 존재
		 *  1-2. 해당 칸을 벗어나는 경우 반대쪽에서 등장
		 * 2. 특수 영양제 투입 후 영양제 삭제. 영양제 투입은 해당 칸의 높이(숫자) 1을 증가
		 * 3. 특수 영양제 투입 칸에 대해 대각선 4방향에 대해 높이 1이상인 칸의 개수 만큼 해당 칸 증가
		 * 	3-1. 대각선이 맵 밖으로 나가는 경우 세지 않음
		 * 4. 특수 영양제를 투입한 칸 제외 높이 2이상인 경우 높이 2를 감소하고 특수 영양제를 올려둠
		 * ...
		 * 5. (1 ~ 4)을 총 M회 만큼 반복하여 M회 후 남아 있는 높이의 총 합 계산
		 */

		POS.push_back(make_pair(N - 2, 0));	// [1-1]
		POS.push_back(make_pair(N - 2, 1));
		POS.push_back(make_pair(N - 1, 0));
		POS.push_back(make_pair(N - 1, 1));

		for (auto& cmd : CMD) {	// [5]
			DEBUG && cout << "[ " << cmd.first << " 방향으로 " << cmd.second << " 만큼 이동 시작 ]\n";
			movePos(cmd);	// [1]
			DEBUG && printArr(TMP, "이동 후 영양제 상황");
			injectPos();	// [2]
			calcDiag();		// [3]
			DEBUG && printArr(MAP, "영양제 증가 후 상황");
			prunMAP();		// [4]
			DEBUG && cout << "영양제 개수: " << POS.size() << "\n";
		}

		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)	answer += MAP[y][x]; 	// [5]
	}

	void input() {
		cin >> N >> M;

		MAP.resize(N, vector<int>(N));
		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)	cin >> MAP[y][x];
		CMD.resize(M);
		for (int i = 0; i < M; ++i)	cin >> CMD[i].first >> CMD[i].second;

		TMP.resize(N, vector<int>(N));
	}
};

int main() {
	Solution s;

	s.input();
	s.solution();

	cout << s.answer;
}