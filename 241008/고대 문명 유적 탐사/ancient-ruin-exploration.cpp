#include <bits/stdc++.h>
#define DEBUG 0

using namespace std;
using pii = pair<int, int>;

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };
const int ddy[] = { -1, -1, -1, 0, 1, 1, 1, 0 };
const int ddx[] = { -1, 0, 1, 1, 1, 0, -1, -1 };

vector<vector<int> > MAP(5, vector<int>(5));
queue<int> WALL;
int K, M;

vector<vector<int> > tmp(5, vector<int>(5));
vector<vector<int> > T(5, vector<int>(5));
int score;
int cnt;

vector<vector<int> > SEC(5, vector<int>(5));
int secId;
bool secFlag[26];

bool printArr(vector<vector<int> >& ARR, string s = "") {
	cout << "\n===== " << s << " =====\n";
	for (int y = 0; y < 5; ++y) {
		for (int x = 0; x < 5; ++x)
			cout << ARR[y][x] << " ";
		putchar('\n');
	}
	putchar('\n');

	return (true);
}

void copyArr(vector<vector<int> >& start, vector<vector<int> >& target) {
	for (int y = 0; y < 5; ++y)	for (int x = 0; x < 5; ++x)
		target[y][x] = start[y][x];
}

bool isValid(int y, int x) {
	if (y < 0 || x < 0 || y >= 5 || x >= 5)	return (false);
	return (true);
}

/**
 * TODO: 중앙 좌표 (y, x)와 회전 값을 주어질 때, 90도 회전한 결과를 tmp 배열에 갱신
 */
void doCycle(int y, int x) {
	queue<int> q;

	for (int flag = 0; flag < 8; ++flag) {
		int ny = y + ddy[flag];
		int nx = x + ddx[flag];
		q.push(tmp[ny][nx]);
	}
	for (int flag = 2; flag < 10; ++flag) {
		int ny = y + ddy[flag % 8];
		int nx = x + ddx[flag % 8];
		tmp[ny][nx] = q.front();
		q.pop();
	}
}

/**
 * TODO: 임시 배열에 대해 1차 가치 반환
 */
int getFirstValue() {
	int val = 0;
	queue<pii> q;

	secId = 0;
	for (int y = 0; y < 5; ++y)	for (int x = 0; x < 5; ++x)	SEC[y][x] = 0;
	memset(secFlag, false, sizeof(secFlag));

	for (int _y = 0; _y < 5; ++_y)
		for (int _x = 0; _x < 5; ++_x)
			if (SEC[_y][_x] == 0) {
				int id = tmp[_y][_x];
				int secCnt = 0;

				q.push(make_pair(_y, _x));
				SEC[_y][_x] = ++secId;
				++secCnt;
				while (!q.empty()) {
					int y = q.front().first;
					int x = q.front().second;
					q.pop();
					
					for (int flag = 0; flag < 4; ++flag) {
						int ny = y + dy[flag];
						int nx = x + dx[flag];

						if (!isValid(ny, nx))	continue;
						if (SEC[ny][nx] > 0 || tmp[ny][nx] != id)	continue;

						q.push(make_pair(ny, nx));
						SEC[ny][nx] = secId;
						++secCnt;
					}
				}

				if (secCnt >= 3) {
					val += secCnt;
					secFlag[secId] = true;
				}	
			}

	return (val);
}

/**
 * TODO: 연쇄작용 후 최종 값을 반환
 */
int getAnswer() {
	int val = 0;
	int rst;

	copyArr(T, tmp);
	while (true) {
		queue<pii> q;

		rst = getFirstValue();
		if (rst == 0)	break;
		val += rst;
		
		DEBUG && printArr(tmp, "MAP 현황");
		DEBUG && printArr(SEC, "section 결과");

		//좌표 추출 (열 번호가 작고 행 번호가 큰 순)
		for (int x = 0; x < 5; ++x)
			for (int y = 4; y >= 0; --y)
				if (secFlag[SEC[y][x]]) {
					q.push(make_pair(y, x));
				}

		if (DEBUG) {
			cout << "검출 section: ";
			for (int flag = 0; flag < 26; ++flag)
				if (secFlag[flag])	cout << flag << " ";
			putchar('\n');
			queue<pii> tq = q;
			cout << "좌표 개수: " << tq.size() << " ";
			while (!tq.empty()) {
				cout << "(" << tq.front().first << "," << tq.front().second << ") ";
				tq.pop();
			}
			putchar('\n');
		}
		

		while (!q.empty()) {
			int y = q.front().first;
			int x = q.front().second;
			q.pop();

			tmp[y][x] = WALL.front();
			WALL.pop();
		}
	}
	return (val);
}

void solution() {
	for (int turn = 1; turn <= K; ++turn) {
		/*
		1. 아래 순서로 좌표 탐색 
			(x) (x) (x) (x) (x)
			(x) (1) (4) (7) (x)
			(x) (2) (5) (8) (x)
			(x) (3) (6) (9) (x)
			(x) (x) (x) (x) (x)
		2. 시계 방향 90 -> 180 -> 270 맵 돌리기
		3. 돌린 후 1차 획득 가치 판단
		...
		4. (1 ~ 3) 과정 반복 후 최대 값, 우선 순위에 맞는 맵 구함
		5-1. 유물이 없다면, 그 즉시 종료
		5-2. 유물이 있다면, 아래 프로세스 진행
		6. (파괴 -> 채워 넣기) 반복
		7. 해당 턴에 파괴된 갯수 출력
		*/

		DEBUG && cout << "\t[ Turn: " << turn << "진행 ]\n";

		for (int x = 1; x <= 3; ++x)	// [1]
			for (int y = 1; y <= 3; ++y) {
				copyArr(MAP, tmp);
				for (int cycle = 1; cycle <= 3; ++cycle) { // [2]
					int t;

					doCycle(y, x);
					//tmp에 회전 후 배열 값 갱신 완료
					t = getFirstValue(); // [3]

					if ((t > score) || (t == score && cycle < cnt)) { // [4]
						score = t;
						cnt = cycle;
						copyArr(tmp, T);
					}
				}
			}
		//T에 1차 획득 가치 가장 큰 배열 모습 담겨 있음
		DEBUG && printArr(T, "90도 각도를 " + to_string(cnt) + " 번 하고 1차 가치가 " + to_string(score) + " 인 최대 배열");
		
		if (score == 0)	break;	// [5-1]
		cout << getAnswer() << " ";	// [5-2, 6], [7]
		
		score = 0;
		copyArr(tmp, MAP);

		DEBUG && printArr(MAP, "Turn " + to_string(turn) + " 종료 후 MAP 모습");
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);	cout.tie(nullptr);

	cin >> K >> M;
	for (int y = 0; y < 5; ++y)	for (int x = 0; x < 5; ++x)	cin >> MAP[y][x];
	for (int i = 0; i < M; ++i) {
		int t;	cin >> t;
		WALL.push(t);
	}

	// copyArr(MAP, tmp);
	// doCycle(2, 2);
	// printArr(tmp, "tmp 값");

	solution();
}


/** 회전 방법
 * 시계방향 90도
 * 시계방향 180도
 * 시계방향 270도
 * (1) 유물 1차 획득 가치 최대화
 * (2) 회전 각도가 가장 작은 방법
 * (3) 회전 중심 좌표: 열이 가장 작은 구간, 열이 같다면 행이 가장 작은 구간
*/

/** 벽 보고 채워 넣는 순서 기준
 * (1) 열 번호가 작은 순
 * (2) 행 번호가 큰 순
*/