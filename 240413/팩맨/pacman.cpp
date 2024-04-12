#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
using namespace std;

#define DEBUG 0

#define SIZE 4
#define UP 0
#define LEFT 2
#define DOWN 4
#define RIGHT 6

const int dy[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int dx[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

typedef struct {
	int y;
	int x;
	int flag;
} Monster;

int M;	//몬스터 마리 수(1 ~10)
int T;	//턴 수(1 ~ 25)
int PY, PX;

vector<vector<unordered_set<int>>> MAP(SIZE, vector<unordered_set<int>>(SIZE));
vector<vector<queue<int>>> DEAD(SIZE, vector<queue<int>>(SIZE));
unordered_map<int, Monster> monsters;
queue<Monster> q;
int order[3];
int bestValue;
int bestOrder[3];
int id;	//1부터 시작
int answer;

string getString(int flag) {
	if (flag == UP)	return ("상");
	else if (flag == 1) 	return ("좌상");
	else if (flag == LEFT)	return ("좌");
	else if (flag == 3)		return ("좌하");
	else if (flag == DOWN)	return ("하");
	else if (flag == 5)		return ("우하");
	else if (flag == RIGHT)	return ("우");
	else	return ("우상");
}

bool printMAP(string s) {
	DEBUG && cout << " ----- " << s << " -----\n";
	for (int _y = 0; _y < 4; ++_y) {
		for (int _x = 0; _x < 4; ++_x) {
			unordered_set<int>& us = MAP[_y][_x];

			if (us.size() > 0)
				DEBUG && cout << " (" << _y << ", " << _x << ") 위치 몬스터\n";
			for (unordered_set<int>::iterator iter = begin(us); iter != end(us); ++iter) {
				Monster& t= monsters[*iter];

				DEBUG && cout << "  id: " << *iter << "  방향: " << getString(t.flag) << "\n";
			}
		}
	}
	cout << "\n\n";

	return (true);
}

bool printDead() {
	DEBUG && cout << " ----- " << "죽은 리스트" << " -----\n";

	for (int _y = 0; _y < 4; ++_y) {
		for (int _x = 0; _x < 4; ++_x) {
			queue<int> qq = DEAD[_y][_x];

			if (qq.size() > 0)
				DEBUG && cout << " (" << _y << ", " << _x << ") 위치 가장 늦게 죽은 Time: " << qq.back() << "\n";
		}
	}	

	DEBUG && cout << "\n\n";
	return (true);
}

bool printCopy() {
	DEBUG && cout << " ----- " << "복사 리스트" << " -----\n";
	queue<Monster> qq = q;
	while (!qq.empty()) {
		Monster t = qq.front();
		qq.pop();

		DEBUG && cout << "(" << t.y << ", " << t.x << ") " << getString(t.flag) << "\n";
	}

	DEBUG && cout << "\n\n";
	return (true);
}

bool isValid(int y, int x) {
	if (y < 0 || x < 0 || y >= SIZE || x >= SIZE)
		return (false);
	return (true);
}

/* TODO: 몬스터 복제 시도
	id에 해당하는 몬스터를 queue(대기열)에 복제한다.
	이후 queue(대기열)에 있는 것을 pop하며 복제 완성한다.
*/
void copyMonster() {
	for (int _y = 0; _y < 4; ++_y)
		for (int _x = 0; _x < 4; ++_x) {
			unordered_set<int>&us = MAP[_y][_x];

			for (unordered_set<int>::iterator iter = begin(us); iter != end(us); ++iter) {
				Monster t = monsters[*iter];
				q.push(t);
			}
		}
}

/* TODO: 몬스터 이동
	현재 방향을 기준으로 1칸 이동한다.
	이동 칸에 시체/팩맨이 있는 경우거나 MAP 밖이라면 반시계 방향으로 45도 회전 후 다시 재확인
		체크: 아직 남아있는 시체는 ALIVE 값이 다름
						X	체크: 시체는 따로 처리 하지 않으므로 현재 turn 값 기준으로 deadTime과 비교해서 죽은 시체인지 확인. 죽은 시체라면 제거까지 함.
		- 만약 8방향을 다 돌았음에도 움직일 수 없으면 움직이지 않음
*/
bool isRemaintDead(queue<int>& deadList, int& turn) {
	if (deadList.empty())	return (false);
	else {
		int deadTime = deadList.back();
		int diff = turn - deadTime;

		if (diff <= 2)
			return (true);
	}

	return (false);
}

void moveMonster(int turn) {
	queue<int> addList;
	
	for (int _y = 0; _y < 4; ++_y) {
		for (int _x = 0; _x < 4; ++_x) {
			unordered_set<int>& us = MAP[_y][_x];
			queue<int> removeList;

			if (us.size() > 0)
				DEBUG && cout << "\n\n각 몬스터 이동 경로 확인\n";
			for (unordered_set<int>::iterator iter = begin(us); iter != end(us); ++iter) {
				Monster& t= monsters[*iter];
				bool ONLY_DEBUG = true;
				DEBUG && cout << " (" << t.y << ", " << t.x << ") ";

				for (int loop = 0; loop < 8; ++loop) {
					int nflag = (t.flag + loop) % 8;
					int ny = t.y + dy[nflag];
					int nx = t.x + dx[nflag];

					if (!isValid(ny, nx))	continue;
					if (ny == PY && nx == PX)	continue;
					if (isRemaintDead(DEAD[ny][nx], turn))	continue;

					removeList.push(*iter);
					t.y = ny;
					t.x = nx;
					t.flag = nflag;
					addList.push(*iter);
					//MAP[t.y][t.x].insert(*iter);

					DEBUG && cout << getString(t.flag) << " 이동 방향으로 이동 -> (" << t.y << ", " << t.x << ")\n";

					ONLY_DEBUG = false;
					break;
				}

				if (ONLY_DEBUG)
					DEBUG && cout << "이동 없음\n";
			}

			while (!removeList.empty()) {
				int target = removeList.front();
				removeList.pop();
				us.erase(target);
			}
		}
	}

	while (!addList.empty()) {
		int target = addList.front();
		Monster t = monsters[target];
		addList.pop();

		MAP[t.y][t.x].insert(target);
	}
}

/* TODO: 팩맨 이동
	4가지 방향 중 3칸을 선택해 이동한다. (총 64)
	우선 순위는 몬스터를 가장 많이 먹을 수 있는 방향으로 이동하며 동일하다면 방향은 상 좌 하 우
	이동하는 과정에서 MAP 밖으로 이동한다면 이동 불가능한다.
	이동 칸들에 존재하는 몬스터는 사라지며, 해당 칸에 시체가 발생한다.
	시작 위치에 있는 몬스터는 먹지 않으며 알 또한 먹지 않는다.
*/
int runOrder(int y, int x) {
	bool visited[4][4] = { {false, }, };
	int result = 0;

	for (int loop = 0; loop < 3; ++loop) {
		int flag = order[loop];
		int ny = y + dy[flag];
		int nx = x + dx[flag];

		if (!isValid(ny, nx))	return (-1);
		
		if (!visited[ny][nx])
			result += MAP[ny][nx].size();
		visited[ny][nx] = true;

		y = ny;
		x = nx;
	}

	return (result);
}

void makePermutation(int depth = 0) {
	if (depth == 3) {
		//순서 조합 완료
		int val = runOrder(PY, PX);		

		DEBUG && cout << getString(order[0]) << " " << getString(order[1]) << " " << getString(order[2]) << " -> " << val << "\n";
	
		if (val > bestValue) {
			bestValue = val;
			bestOrder[0] = order[0];
			bestOrder[1] = order[1];
			bestOrder[2] = order[2];
		}
		return;
	}
	for (int flag = 0; flag <= 6; flag += 2) {
		order[depth] = flag;
		makePermutation(depth + 1);
	} 
}

void changeDead(int y, int x, int& turn) {
	unordered_set<int>& us = MAP[y][x];

	if (us.size() > 0)
		DEAD[y][x].push(turn);
	for (unordered_set<int>::iterator iter = begin(us); iter != end(us); ++iter)
		monsters.erase(*iter);
	us.clear();
}

void moveCather(int turn) {
	bestValue = -1;
	makePermutation();

	DEBUG && cout << "\n최적의 이동 경로: " << getString(bestOrder[0]) << " " << getString(bestOrder[1]) << " " << getString(bestOrder[2]) << "\n";
	DEBUG && cout << "이동 위치 몬스터 총 수: " << bestValue << "\n\n";

	answer -= bestValue;
	for (int loop = 0; loop < 3; ++loop) {
		int flag = bestOrder[loop];
		
		PY += dy[flag];
		PX += dx[flag];

		changeDead(PY, PX, turn);
	}
}

/* TODO: 몬스터 시체 소멸
*/

/* TODO: 몬스터 복제 완성
	answer를 queue(대기열)만큼 증가한다.
	queue(대기열)에 있는 것을 MAP에 추가하고 monsters에 ++id값으로 추가한다.
*/
void runCopyMonster() {
	answer += q.size();
	while (!q.empty()) {
		Monster t = q.front();
		q.pop();

		monsters[++id] = t;
		MAP[t.y][t.x].insert(id);
	}
}

void solution() {
	DEBUG && cout << "초기 생존자: " << answer << "\n\n";
	DEBUG && printMAP("초기 MAP");
	for (int turn = 1; turn <= T; ++turn) {
		DEBUG && cout << "\n      [" << turn << " 턴 시작] ->  팩맨 위치 (" << PY << ", " << PX << ")\n";
		copyMonster();			DEBUG && printCopy();
		moveMonster(turn);		DEBUG && printMAP("이동 후 MAP");
		moveCather(turn);		DEBUG && printDead();
		runCopyMonster();		DEBUG && printMAP("알 부화 후 MAP");

		DEBUG && cout << "Turn[" << turn << "]" << " 종료 후 생존자 값: " << answer << "\n";
	}
}

void input() {
	cin >> M >> T;
	cin >> PY >> PX;

	--PY;
	--PX;
	for (int i = 0; i < M; ++i) {
		Monster monster;
		cin >> monster.y >> monster.x >> monster.flag;
	
		--monster.y;
		--monster.x;
		--monster.flag;

		monsters[++id] = monster;
		MAP[monster.y][monster.x].insert(id);
	}

	answer = M;	
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	input();
	solution();

	cout << answer;
}