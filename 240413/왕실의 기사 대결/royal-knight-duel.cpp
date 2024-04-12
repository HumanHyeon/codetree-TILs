#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;
using pii = pair<int, int>;

#define MAX 41

#define EMPTY 0
#define TRAP 1
#define WALL 2

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

const int dy[] = { -1, 0, 1, 0 };
const int dx[] = { 0, 1, 0, -1 };

typedef struct {
	int y;
	int x;
	int height;	//높이
	int weight;	//넓이
	int k;	//초기 체력. 절대 수정하지 않음
	int health;	//체력(1~100)
} Node;

int L;	//체스판의 크기(3~40)
int N;	//기사의 수(1~30)
int Q;	//명령의 수(1~100)

int MAP[MAX][MAX];
vector<Node> GISA;
vector<pii> COMMAND;
int answer;	//생존한 기사들이 받은 총 데미지의 합

unordered_map<int, Node> REF;

bool isValid(int y, int x) {
	if (y < 0 || x < 0 || y >= L || x >= L)
		return (false);
	return (true);
}

/* TODO: 주어진 좌표에 위치한 기사의 ID(0~(N-1))를 반환한다.
	- 위치한 기사가 없으면 -1을 반환한다.
	- health가 0이하인 죽은 기사는 반환하지 않는다.
*/
int checkRange(int y, int x) {
	/*	범위는 [y1, y2)	[x1, x2)
		(y1, x1)	(y1, x2)
		(y2, x1)	(y2, x2)
	*/
	for (int i = 0; i < N; ++i) {
		int y1 = GISA[i].y;
		int x1 = GISA[i].x;
		int y2 = GISA[i].y + GISA[i].height;
		int x2 = GISA[i].x + GISA[i].weight;

		if (GISA[i].health <= 0)	continue;
		if ((y1 <= y && y < y2) && (x1 <= x && x < x2))
			return (i);
	}
	return (-1);
}

/* TODO: vPos위치 기준으로 w x h 범위 내 함정 수를 계산하여 반환한다. */
int getDamage(const pii& vPos, const int& id) {
	const Node& t = GISA[id];
	int damage = 0;

	for (int y = vPos.first; y < vPos.first + t.height; ++y)
		for (int x = vPos.second; x < vPos.second + t.weight; ++x)
			if (MAP[y][x] == TRAP)
				++damage;

	return (damage);
}

/* TODO: id에 해당하는 기사를 flag 방향으로 민 후 좌표값들을 반환한다.
	-(X) 1. 죽은 기사의 id 이거나 2. 맵 밖으로 이동하거나 3. 벽이 있는 좌표의 경우 담지 않는다.
 */
vector<pii> getvPosList(const int& id, const int& flag) {
	const Node& t = GISA[id];
	vector<pii> vPosList, empty;
	pii vPos;

//	if (t.health <= 0)	return (empty);

	if (flag == UP) {
		vPos.first = t.y + dy[flag];
		
//		if (!isValid(vPos.first, t.x))	return (empty);
		
		for (int i = 0; i < t.weight; ++i) {
			vPos.second = t.x + i;
//			if (MAP[vPos.first][vPos.second] == WALL)	return (empty);
			vPosList.push_back(vPos);
		}
	}
	else if (flag == RIGHT) {
		vPos.second = t.x + t.weight - 1 + dx[flag];

//		if (!isValid(t.y, vPos.second))	return (empty);

		for (int i = 0; i < t.height; ++i) {
			vPos.first = t.y + i;
//			if (MAP[vPos.first][vPos.second] == WALL)	return (empty);
			vPosList.push_back(vPos);
		}
	}
	else if (flag == DOWN) {
		vPos.first = t.y + t.height - 1 + dy[flag];

//		if (!isValid(vPos.first, t.x))	return (empty);

		for (int i = 0; i < t.weight; ++i) {
			vPos.second = t.x + i;
//			if (MAP[vPos.first][vPos.second] == WALL)	return (empty);
			vPosList.push_back(vPos);
		}
	}
	else {	//LEFT
		vPos.second = t.x + dx[flag];

//		if (!isValid(t.y, vPos.second))	return (empty);

		for (int i = 0; i < t.height; ++i) {
			vPos.first = t.y + i;
//			if (MAP[vPos.first][vPos.second] == WALL)	return (empty);
			vPosList.push_back(vPos);
		}
	}

	return (vPosList);
}

/* TODO: 재귀 함수로 id에 해당하는 기사를 flag 방향으로 민다. 결과는 REF 해쉬맵에 담아둔다.
	- 밀려나는 곳들을 반복문을 돌려 기사(id)가 있으면 동일한 재귀 함수를 호출하여 결과를 가져온다.
		- 단, 한 번이라도 false가 return 되면 그 즉시 종료한다.
		- REF 해쉬맵을 참고하여 이미 밀었던 전적이 있으면 재귀 함수를 호출하지 않는다.
	- std와 id 값이 동일한 기사라면 데미지를 받지 않는다.
	- 밀려날 수 있다면 REF 해쉬맵에 밀려난 후 정보를 담는다.
	- 밀려날 수 없다면 false를 reutrn 한다.
*/
bool pullGISA(int id, const int& flag, const int& std) {
	vector<pii> vPosList;
	Node node;

	vPosList = getvPosList(id, flag);
	for (int i = 0; i < vPosList.size(); ++i) {
		const pii& vPos = vPosList[i];
		int targetId;

		if (!isValid(vPos.first, vPos.second)) 	return (false);
		if (MAP[vPos.first][vPos.second] == WALL)	return (false);
		
		targetId = checkRange(vPos.first, vPos.second);
		if (targetId != -1) {	//해당하는 위치에 기사 있음.
			bool rst; 

			if (REF.find(targetId) != REF.end()) 	continue;
			
			rst = pullGISA(targetId, flag, std);
			if (!rst)
				return (false);
		}
	}

	node.y = GISA[id].y + dy[flag];
	node.x = GISA[id].x + dx[flag];	
	node.height = GISA[id].height;
	node.weight = GISA[id].weight;
	node.k = GISA[id].k;

	if (std == id)	node.health = GISA[id].health;
	else			node.health = GISA[id].health - getDamage(make_pair(node.y, node.x), id);

	REF[id] = node;

	return (true);
}

void solution() {
	for (vector<pii>::iterator iter = begin(COMMAND); iter != end(COMMAND); ++iter) {
		int id = iter->first;
		int flag = iter->second;
		bool rst;

		if (GISA[id].health <= 0)	continue;

		rst = pullGISA(id, flag, id);
		if (rst) {
			for (unordered_map<int, Node>::iterator iter = begin(REF); iter != end(REF); ++iter)
				GISA[iter->first] = iter->second;
		}
		REF.clear();
	}

	//정답 계산: 생존한 기사들이 받은 데미지의 합
	for (vector<Node>::iterator iter = begin(GISA); iter != end(GISA); ++iter) {
		if (iter->health <= 0)	continue;
		else 	answer += (iter->k - iter->health);
	}
}

void input() {
	cin >> L >> N >> Q;

	for (int y = 0; y < L; ++y)
		for (int x = 0; x < L; ++x)
			cin >> MAP[y][x];

	GISA = vector<Node>(N);
	for (int i = 0; i < N; ++i) {
		cin >> GISA[i].y >> GISA[i].x >> GISA[i].height >> GISA[i].weight >> GISA[i].k;
		--GISA[i].y;
		--GISA[i].x;
		GISA[i].health = GISA[i].k;
	}

	COMMAND = vector<pii>(Q);
	for (int i = 0; i < Q; ++i) {
		cin >> COMMAND[i].first >> COMMAND[i].second;
		--COMMAND[i].first;
	}		
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	input();
	solution();

	cout << answer;
}