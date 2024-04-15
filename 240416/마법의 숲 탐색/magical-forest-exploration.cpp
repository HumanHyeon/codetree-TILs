#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;
using pii = pair<int, int>;

#define DEBUG 0
#define DOUT DEBUG&&cout

#define MAX 71
#define MAX_K 1001

#define EMPTY -1

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

typedef struct {
	int y;
	int x;
	int flag;
} Node;

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };

int N;	//세로R (5~70)
int M;	//가로C	(5~70)
int K;	//우주선 수(1~1000)

int MAP[MAX][MAX];
pii COMMAND[MAX_K];	//출발하는 열, 탈출 방향

Node AUZU[MAX_K];

int answer;

bool printMAP(string s) {
	DOUT << "----- " << s << " -----\n";
	for (int y = 0; y < N; ++y) {
		for (int x = 0; x < M; ++x) {
			printf("%3d ", MAP[y][x]);
		}
		DOUT << "\n";
	}
	DOUT << "\n";

	return (true);
}

pii getExitPos(const Node& t) {
	pii pos;

	pos.first = t.y + dy[t.flag];
	pos.second = t.x + dx[t.flag];

	return (pos);
}

int getClockwise(int flag) {
	return ((flag + 1) % 4);
}

int getCounterClockwise(int flag) {
	--flag;
	if (flag < 0)	return (LEFT);
	else 			return (flag);
}

bool isValid(int y, int x) {
	if (y <= 0) {
		if (x < 0 || x >= M)	return (false);
		return (true);
	}
	else {
		if (y < 0 || x < 0 || y >= N || x >= M)	
			return (false);
		for (int flag = 0; flag < 4; ++flag) {
			int ny = y + dy[flag];
			int nx = x + dx[flag];

			if (ny < 0 || nx < 0 || ny >= N || nx >= M)	
				return (false);
		}
		return (true);
	}
}

bool checkNearby(int y, int x) {
	if (y == -2)
		return (false);
	else if (y == -1) {
		if (MAP[y + dy[DOWN]][x] != EMPTY)	return (true);
		else		 						return (false);
	}	
	else if (y == 0) {
		if (MAP[y][x] != EMPTY)						return (true);
		else if (MAP[y][x + dx[LEFT]] != EMPTY)		return (true);	
		else if (MAP[y][x + dx[RIGHT]] != EMPTY)	return (true);
		else if (MAP[y + dy[DOWN]][x] != EMPTY)		return (true);
		
		return (false);
	}
	else {
		if (MAP[y][x] != EMPTY)	return (true);
		for (int flag = 0; flag < 4; ++flag) {
			int ny = y + dy[flag];
			int nx = x + dx[flag];

			if (MAP[ny][nx] != EMPTY)	return (true);
		}
		return (false);
	}
}

Node dropOne(int y, int x, int flag) {
	Node node;
	int ny, nx;
	bool check;

	if (y == N - 2) {
		Node node;

		node.y = y;
		node.x = x;
		node.flag = flag;

		return (node);
	}

	//1. 아래로 이동
	ny = y + dy[DOWN];
	nx = x;
	check = checkNearby(ny, nx);
	
	if (!check)
		return (dropOne(ny, nx, flag));

	//2. 왼쪽 이동
	check = false;

	ny = y;
	nx = x + dx[LEFT];
	if (!isValid(ny, nx))
		check = true;
	else {
		bool rst = checkNearby(ny, nx);
		if (rst)	check = true;
	}
	
	ny = y + dy[DOWN];
	nx = x + dx[LEFT];
	if (!isValid(ny, nx))
		check = true;
	else {
		bool rst = checkNearby(ny, nx);
		if (rst)	check = true;
	}

	if (!check)
		return (dropOne(ny, nx, getCounterClockwise(flag)));

	//3. 오른쪽 이동
	check = false;

	ny = y;
	nx = x + dx[RIGHT];
	if (!isValid(ny, nx))
		check = true;
	else {
		bool rst = checkNearby(ny, nx);
		if (rst)	check = true;
	}

	ny = y + dy[DOWN];
	nx = x + dx[RIGHT];
	if (!isValid(ny, nx))
		check = true;
	else {
		bool rst = checkNearby(ny, nx);
		if (rst)	check = true;
	}

	if (!check)
		return (dropOne(ny, nx, getClockwise(flag)));
	
	//4. 이동 불가능
	node.y = y;
	node.x = x;
	node.flag = flag;
	
	return (node);
}

int getScore(const Node& t, int _id) {
	queue<int> q;
	bool visited[MAX_K] = { false, };
	int score;

	score = t.y + 1;
	visited[_id] = true;
	q.push(_id);
	while (!q.empty()) {
		int id = q.front();
		pii exitPos;
		q.pop();

		score = max(score, AUZU[id].y + 1);

		exitPos = getExitPos(AUZU[id]);
		for (int flag = 0; flag < 4; ++flag) {
			int ny = exitPos.first + dy[flag];
			int nx = exitPos.second + dx[flag];
			int targetId;

			if (ny < 0 || nx < 0 || ny >= N || nx >= M)	continue;
			if (MAP[ny][nx] == EMPTY)	continue;

			targetId = MAP[ny][nx];
			if (visited[targetId])	continue;

			visited[targetId] = true;
			q.push(targetId);
		}
	}

	return (score + 1);
}

void solution() {
	DEBUG && printMAP("최초 MAP");

	for (int id = 0; id < K; ++id) {
		pii cmd = COMMAND[id];
		Node t;

		t = dropOne(-2, cmd.first, cmd.second);

		if (t.y <= 0) {
			DOUT << "[solution] 놓을 수 없는 상황 발생. 초기화 진행\n";
			for (int y = 0; y < N; ++y)
				for (int x = 0; x < M; ++x)
					MAP[y][x] = EMPTY;
		}
		else {	//놓을 수 있음.
			int score;

			MAP[t.y][t.x] = id;
			for (int flag = 0; flag < 4; ++flag) {
				int ny = t.y + dy[flag];
				int nx = t.x + dx[flag];

				MAP[ny][nx] = id;
			}
			AUZU[id] = t;

			score = getScore(t, id);

			DOUT << "[solution] 이번 턴에 얻은 점수: " << score << "\n";
			answer += score;
 		}

		DOUT << "이번 턴에 놓은 우주선 정보: (" << AUZU[id].y << ", " << AUZU[id].x << ")  flag: " << AUZU[id].flag << "\n";
		DEBUG && printMAP(to_string(id) + " 놓은 후 MAP 모습");
	}
}

void init() {
	for (int y = 0; y < N; ++y)
		for (int x = 0; x < M; ++x)
			MAP[y][x] = EMPTY;
}

void input() {
	cin >> N >> M >> K;
	for (int i = 0; i < K; ++i) {
		cin >> COMMAND[i].first >> COMMAND[i].second;
		--COMMAND[i].first;
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	input();
	init();
	solution();

	cout << answer;
}