#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <cstring>
#include <cstdio>
using namespace std;

#define DEBUG 0
#define DOUT DEBUG && cout

#define MAX 21
#define PLAYER_MAX 31

#define EMPTY -1

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

typedef struct {
	int y;
	int x;
	int flag;	//방향 상우하좌 순
	int strong;	//초기 능력치
	int gun;	//가지고 있는 gun 공격력 (0은 총이 없음)
} Node;

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };

int N;	//격자 크기(2~20)
int M;	//플레이어 수 (1~min(n^2,30))
int K;	//라운드 수(1~500)

priority_queue<int> MAP[MAX][MAX];
Node PLAYER[PLAYER_MAX];
int REF[MAX][MAX];	//플레이어 위치 정보(ID값으로)

int answer[PLAYER_MAX];

string getString(int flag) {
	if (flag == UP)	return ("위");
	else if (flag == RIGHT)	return ("오른쪽");
	else if (flag == DOWN)	return ("아래");
	else 	return ("왼쪽");
}

bool printREF(string s) {
	DOUT << "------ " << s << " ------\n";
	for (int y = 0; y < N; ++y) {
		for (int x = 0; x < N; ++x) 
			printf("%3d ", REF[y][x]);
		DOUT << "\n";
	}
	DOUT << "\n";

	return (true);
}

bool isValid(int y, int x) {
	if (y < 0 || x < 0 || y >= N || x >= N)
		return (false);
	return (true);
}

int getOpposite(int flag) {
	return ((flag + 2) % 4);
}

int getClockwise(int flag) {
	return ((flag + 1) % 4);
}

void updateREF(int ny, int nx, int id) {
	Node& t = PLAYER[id];
	
	REF[t.y][t.x] = EMPTY;
	REF[ny][nx] = id;
}

void moveLoser(int id, int _y, int _x) {
	Node& t = PLAYER[id];
	int ny, nx;

	if (t.gun != 0) {
		MAP[_y][_x].push(t.gun);
		t.gun = 0;
	}

	for (int loop = 0; loop < 4; ++loop) {
		ny = _y + dy[t.flag];
		nx = _x + dx[t.flag];

		if (!isValid(ny, nx)) {
			t.flag = getClockwise(t.flag);
			continue;
		}
		if (REF[ny][nx] != EMPTY) {
			t.flag = getClockwise(t.flag);
			continue;
		}
		break;
	}

	DOUT << "[moveLoser] " << "paraPos: (" << _y << ", " << _x << ")  -> 이동할 위치: (" << ny << ", " << nx << ")\n";

	REF[ny][nx] = id;
	t.y = ny;
	t.x = nx;
	if (!MAP[ny][nx].empty()) {
		t.gun = MAP[ny][nx].top();
		MAP[ny][nx].pop();
	}
}

void run() {
	for (int i = 0; i < M; ++i) {
		DOUT << "[" << i << "] 번 플레이어 시작\n"; 
		DEBUG && printREF("REF 정보");
	
		Node& t = PLAYER[i];
		int ny = t.y + dy[t.flag];
		int nx = t.x + dx[t.flag];

		if (!isValid(ny, nx)) {	//격자 밖일 때는 반대 방향으로 1칸 이동
			DOUT << "다음 칸이 격자 밖이므로 반대 방향으로 이동\n";
			t.flag = getOpposite(t.flag);
			ny = t.y + dy[t.flag];
			nx = t.x + dx[t.flag];			
		}

		DOUT << "이동 위치: (" << ny << ", " << nx << ")\n";
		
		if (REF[ny][nx] == EMPTY) {
			if (t.gun != 0)	MAP[ny][nx].push(t.gun);
			
			if (!MAP[ny][nx].empty()) {
				t.gun = MAP[ny][nx].top();
				MAP[ny][nx].pop();
			}
			
			updateREF(ny, nx, i);
			t.y = ny;
			t.x = nx;
			DOUT << "이동 위치 비어 있음. 바로 이동. 플레이어 정보: strong=" << t.strong << " flag=" << getString(t.flag) << " gun=" << t.gun << "\n";
		}
		else {	//경합 시나리오
			DOUT << "경합 시나리오\n";

			Node& cmp = PLAYER[REF[ny][nx]];
			int cmpId = REF[ny][nx];
			int mAttack = t.strong + t.gun;
			int cmpAttack = cmp.strong + cmp.gun;
			bool check; //t(이동자)가 이기면 True, cmp(기존자)가 이기면 False

			if (mAttack > cmpAttack)		check = true;
			else if (mAttack < cmpAttack)	check = false;
			else {
				if (t.strong > cmp.strong)	check = true;
				else						check = false;
			}

			//이긴 플레이어는 각 플레이어의 초기 능력치와 가자고 있는 총의 공격력의 합의 차이만큼 포인트 획득

			//반드시 REF정보 갱신해주고 moveLoser호출하기
			if (check) {	//cmp(기존자)가 이동
				DOUT << "기존자(" << REF[ny][nx] << ")가 이동  기존자: strong= " << cmp.strong << "  총= " << cmp.gun << "  이동자: strong= " << t.strong<< "  총= " << t.gun << "\n";
				answer[i] += (mAttack - cmpAttack);

				updateREF(ny, nx, i);
				t.y = ny;
				t.x = nx;
				moveLoser(cmpId, ny, nx);		
				
				if (t.gun != 0)	MAP[ny][nx].push(t.gun);

				if (!MAP[ny][nx].empty()) {
					t.gun = MAP[ny][nx].top();
					MAP[ny][nx].pop();					
				}
			}
			else {	//t(이동자)가 이동
				DOUT << "이동자(" << i << ")가 이동   기존자: strong= " << cmp.strong << "  총= " << cmp.gun << "  이동자: strong= " << t.strong<< "  총= " << t.gun << "\n";
				answer[REF[ny][nx]] += (cmpAttack - mAttack);

				REF[t.y][t.x] = EMPTY;
				moveLoser(i, ny, nx);

				if (cmp.gun != 0)	MAP[ny][nx].push(cmp.gun);

				if (!MAP[ny][nx].empty()) {
					cmp.gun = MAP[ny][nx].top();
					MAP[ny][nx].pop();
				}
			}
		}
		DOUT << "\n";
	}
}

void solution() {
	for (int turn = 1; turn <= K; ++turn) {
		DOUT << "===== " << turn << " 턴 시작 ======\n";
		run();
		DOUT << "\n";
	}
	DEBUG && printREF("모든 턴 종료 후");
}

void input() {
	cin >> N >> M >> K;

	for (int y = 0; y < N; ++y)
		for (int x = 0; x < N; ++x) {
			int gun;
			cin >> gun;

			MAP[y][x].push(gun);
		}

	memset(REF, EMPTY, sizeof(REF));

	for (int i = 0; i < M; ++i) {
		cin >> PLAYER[i].y >> PLAYER[i].x >> PLAYER[i].flag >> PLAYER[i].strong;

		--PLAYER[i].y;
		--PLAYER[i].x;
		PLAYER[i].gun = 0;

		REF[PLAYER[i].y][PLAYER[i].x] = i;
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	input();
	solution();

	for (int i = 0; i < M; ++i)
		cout << answer[i] << " ";
}