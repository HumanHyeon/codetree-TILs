#include <bits/stdc++.h>
using namespace std;

using vvi = vector<vector<int> >;
using vi = vector<int>;
using pii = pair<int, int>;

#define DEBUG 0
#define SIZE 31

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };

class Solution {
public:
	int N;
	vvi MAP;
	int answer;

	vvi SEC;
	vi secCnt;
	vector<pii> POS;
	vector<vector<bool> > visited;

	Solution() : answer(0) {}

	void input() {
		cin >> N;
		MAP.resize(N + 2, vi(N + 2));
		SEC.resize(N + 2, vi(N + 2));
		secCnt.resize(N * N + 2);
		POS.resize(N * N + 2);
		visited.resize(N + 2, vector<bool>(N + 2));
		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)	cin >> MAP[y][x];
	}

	bool printArr(vvi& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x)
				printf("%2d ", T[y][x]);
			putchar('\n');
		}
		putchar('\n');
		return (true);
	}

	void copyArr(const vvi& start, vvi& target) { 
		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)	target[y][x] = start[y][x];
	}

	bool isValid(int y, int x) {
		if (y < 0 || x < 0 || y >= N || x >= N)	return (false);
		return (true);
	}

	/**
	 * TODO: 현재 MAP의 예술성 점수를 구해서 반환
	 */
	int getScore() {
		int score = 0;
		int secId = 0;

		for (int y = 0; y < N; ++y)	for (int x = 0; x < N; ++x)
			SEC[y][x] = secCnt[y * N + x + 1] = 0;

		for (int _y = 0; _y < N; ++_y) // [1-1]
			for (int _x = 0; _x < N; ++_x)
				if (SEC[_y][_x] == 0) {
					queue<pii> q;
					int id = MAP[_y][_x];
					
					q.push(make_pair(_y, _x));
					SEC[_y][_x] = ++secId;
					secCnt[secId] = 1;
					POS[secId] = make_pair(_y, _x);
					while (!q.empty()) {
						int y = q.front().first;
						int x = q.front().second;
						q.pop();

						for (int flag = 0; flag < 4; ++flag) {
							int ny = y + dy[flag];
							int nx = x + dx[flag];

							if (!isValid(ny, nx))	continue;
							if (SEC[ny][nx] != 0 || MAP[ny][nx] != id)	continue;

							q.push(make_pair(ny, nx));
							SEC[ny][nx] = secId;
							++secCnt[secId];	// [1-2]
						}
					}
				}

		// 그룹핑 끝
		DEBUG && printArr(SEC, "Section 정보");
		if (DEBUG) {
			DEBUG && cout << "section 개수 ";
			for (int i = 1; i <= secId; ++i) 
				cout << "(" << i << ": " << secCnt[i] << ") ";
			putchar('\n');
			DEBUG && cout << "sec 좌표: ";
			for (int i = 1; i <= secId; ++i) 
				cout << "sec " << i << " " << "(" << POS[i].first << ", " << POS[i].second << ") ";
			putchar('\n');
		} 

		for (int id = 1; id < secId; ++id) { // [1.3]
			pii& pos = POS[id];
			queue<pii> q;
			map<int, int> near;

			DEBUG && cout << "id: " << id << " GO!\n";

			for (int y = 0; y < visited.size(); ++y) for (int x = 0; x < visited[0].size(); ++x)	visited[y][x] = false;

			q.push(pos);
			visited[pos.first][pos.second] = true;
			while (!q.empty()) {
				int y = q.front().first;
				int x = q.front().second;
				q.pop();

				for (int flag = 0; flag < 4; ++flag) {
					int ny = y + dy[flag];
					int nx = x + dx[flag];

					if (!isValid(ny, nx))	continue;
					if (visited[ny][nx])	continue;

					if (SEC[ny][nx] < id) continue;
					else if (SEC[ny][nx] == id) {
						q.push(make_pair(ny, nx));
						visited[ny][nx] = true;
					}
					else {
						DEBUG && cout << "\t맞닿은 변 발견! SEC: (" << id << ", " << SEC[ny][nx] << ")  y: " << y << " x: " << x << " ny: " << ny << " nx: " << nx << "\n"; 
						++near[SEC[ny][nx]];
					}
				}
			}

			for (auto iter = begin(near); iter != end(near); ++iter) {
				int nearId = iter->first;
				int nearCnt = iter->second;
				pii& nearPos = POS[nearId];

				int t = (secCnt[id] + secCnt[nearId]) * MAP[pos.first][pos.second] * MAP[nearPos.first][nearPos.second] * nearCnt;
				DEBUG && cout << "nearId: " << nearId << "  nearCnt: " << nearCnt << " t: (" << secCnt[id] << " + " << secCnt[nearId] << ") * " << MAP[pos.first][pos.second] << " * " << MAP[nearPos.first][nearPos.second] << " * " << nearCnt << " = " << t << "\n";
				score += t;
			}
		}	

		return (score);
	} 

	void runMiniCycle(int _y, int _x, int len) {
		DEBUG && cout << " >>>>> runMiniCycle 진입  startY: " << _y << " startX: " << _x << "  len: " << len << "\n";
		queue<int> q;

		for (int y = 0; y < len; ++y)
			for (int x = y + 1; x < len; ++x) {
				//cout << "y: " << y << "  x: " << x << "\n";
				swap(MAP[_y + y][_x + x], MAP[_x + x][_y + y]);
			}
		printArr(MAP, "T1");
		for (int y = _y; y < _y + len; ++y)
			for (int x = _x; x < _x + (len / 2); ++x) {
				swap(MAP[y][x], MAP[y][_x + len - x]);
			}
		printArr(MAP, "T2");
	}

	/**
	 * TODO: 현재 MAP을 2. 조건에 맞게 Cycle, MAP에 Update
	 */
	void doCycle() {
		queue<int> q;
		int len = N / 2;
		int y = N / 2;
		int x = N / 2;

		for (int i = 1; i <= len; ++i) {
			q.push(MAP[y - i][x]);
			q.push(MAP[y][x - i]);
			q.push(MAP[y + i][x]);
			q.push(MAP[y][x + i]);

			MAP[y][x - i] = q.front();	q.pop();
			MAP[y + i][x] = q.front(); 	q.pop();
			MAP[y][x + i] = q.front(); 	q.pop();
			MAP[y - i][x] = q.front();	q.pop();
		}

		for (int y = 0; y < len; ++y)	for (int x = 0; x < len; ++x)	q.push(MAP[y][x]);
		for (int x = len - 1; x >= 0; --x)	for (int y = 0; y < len; ++y) { MAP[y][x] = q.front(); q.pop(); }

		for (int y = 0; y < len; ++y)	for (int x = len + 1; x < N; ++x)	q.push(MAP[y][x]);
		for (int x = N - 1; x > len; --x)	for (int y = 0; y < len; ++y) { MAP[y][x] = q.front(); q.pop(); }

		for (int y = len + 1; y < N; ++y) for (int x = 0; x < len; ++x)	q.push(MAP[y][x]);
		for (int x = len - 1; x >= 0; --x)	for (int y = len + 1; y < N; ++y) { MAP[y][x] = q.front(); q.pop(); }

		for (int y = len + 1; y < N; ++y) for (int x = len + 1; x < N; ++x)	q.push(MAP[y][x]);
		for (int x = N - 1; x > len; --x)	for (int y = len + 1; y < N; ++y) { MAP[y][x] = q.front(); q.pop(); }
	}

	void solution() {
		/**
		 * 1. MAP에서 예술성 점수를 구함
		 * 	1-1. 상하좌우 인접한 같은 숫자을 그룹핑 (FloodFill)
		 *  1-2. 계산 과정에서 그룹별 소속한 갯수 구해놓기
		 *  1-3. 인접한 그룹끼리 조화로움 계산식으로 점수 계산
		 * 	 ㄴ 계산식: (A 그룹에 속한 개수 + B 그룹에 속한 개수) x 그룹 A에 대한 값 x 그룹 B에 대한 값 x A, B그룹이 맞닿아 있는 변의 수
		 * 2. MAP 회전
		 * 	2-1. 십자 모양의 경우 반시계(왼쪽 90도) 회전
		 * 	2-2. 나머지 4구역은 시계(오른쪽 90도) 회전
		 * ...
		 * 3. (1 ~ 2) 3회 반복 후 나온 예술성 점수 합산
		*/

		DEBUG && printArr(MAP, "초기 MAP Status");
		for (int i = 1; i <= 4; ++i) {
			int score = getScore();	// [1]
			doCycle(); // [2]
			answer += score; // [3]
			DEBUG && printArr(MAP, to_string(i) + "회전 후 MAP Status (이번 턴 획득 점수: " + to_string(score) + ")");
			DEBUG && cout << "총 점수: " << answer << "\n";
		}
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	for (int tc = 1; tc <= 1; ++tc) {
		Solution s;

		s.input();
		s.solution();
		cout << s.answer << "\n";
	}
}