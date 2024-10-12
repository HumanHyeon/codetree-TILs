#include <bits/stdc++.h>
#include <unordered_set>
using namespace std;
using pii = pair<int, int>;

#define DEBUG 0

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 }; 

const int SCORE[] = { 0, 1, 10, 100, 1000 };

class Solution {
public:
	int answer;
	int N;

	vector<vector<int> > MAP;
	vector<unordered_set<int> > REF;
	queue<int> ORDER;

	bool printArr(vector<vector<int> >& T, string s = "") {
		cout << " ===== " << s << " =====\n";
		for (int y = 0; y < N; ++y)	{
			for (int x = 0; x < N; ++x)
				printf("%3d ", T[y][x]);
			putchar('\n');
		}
		putchar('\n');

		return (true);
	}

	inline bool isValid(int y, int x) {
		return (!(y < 0 || x < 0 || y >= N || x >= N));
	}

	Solution() : answer(0) {}

	void setPos(int order) {
		pii pos = make_pair(-1, -1);
		int good = -1;
		int empty = -1;

		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x) {
				const unordered_set<int>& ref = REF[order];
				int go = 0;
				int emt = 0;

				if (MAP[y][x] != -1)	continue;

				for (int flag = 0; flag < 4; ++flag) {
					int ny = y + dy[flag];
					int nx = x + dx[flag];

					if (!isValid(ny, nx))	continue;

					if (MAP[ny][nx] == -1)	++emt;
					else if (ref.find(MAP[ny][nx]) != end(ref)) ++go;
				}

				if (go > good) {
					good = go;
					empty = emt;
					pos = make_pair(y, x);
				}
				else if (go == good) {
					if (emt > empty) {
						good = go;
						empty = emt;
						pos = make_pair(y, x);	
					}
				}
			}
		}

		MAP[pos.first][pos.second] = order;
	}

	void solution() {
		DEBUG && printArr(MAP, "초기 상태");

		/**
		 * 1. 전체 맵을 탐색하면서 인접한 4칸 중 좋아하는 친구가 많은 위치로 이동
		 * 	1-1. 격자를 벗어나면 체크하지 않음
		 * 2. 1이 여러개라면, 인접한 칸들 중 비어있는 칸이 가장 많은 위치로 이동
		 * 	2-2. 격자를 벗어나는 칸은 체크하지 않음
		 * 3. 2가 여러개라면 행이 가장 작은 위치로
		 * 4. 3이 여러개라면 열이 가장 작은 위치로
		 */

		while (!ORDER.empty()) {
			int order = ORDER.front();	ORDER.pop();

			setPos(order);
			DEBUG && printArr(MAP, to_string(order) + " 가 놓인 후 MAP 상태");
		}

		for (int y = 0; y < N; ++y)
			for (int x = 0; x < N; ++x) {
				const unordered_set<int>& ref = REF[MAP[y][x]];
				int cnt = 0;

				for (int flag = 0; flag < 4; ++flag) {
					int ny = y + dy[flag];
					int nx = x + dx[flag];

					if (!isValid(ny, nx))	continue;
					if (ref.find(MAP[ny][nx]) != end(ref))	++cnt;
				}

				DEBUG && cout << " y: " << y << " x: " << x << "  score = " << SCORE[cnt] << "\n";
				answer += SCORE[cnt];
			}
	}

	void input() {
		int len;
		cin >> N;
		MAP.resize(N, vector<int>(N, -1));
		REF.resize(N * N);
		len = N * N;
		for (int y = 0; y < len; ++y) {
			int idx;	cin >> idx;
			ORDER.push(idx - 1);
			for (int x = 0; x < 4; ++x)	{
				int t;	cin >> t;
				REF[idx - 1].insert(t - 1);
			}
		} 
	}
};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	 cout.tie(NULL);

	for (int tc = 1; tc <= 1; ++tc) {
		Solution s;
		s.input();
		s.solution();
		cout << s.answer;
	}
}