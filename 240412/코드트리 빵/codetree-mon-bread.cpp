#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>
#include <cmath>
using namespace std;
using pii = pair<int, int>;

#define MAX 16

typedef struct {
	int id;
	int y;
	int x;
	int targetY;
	int targetX;
} Node;

typedef struct {
	int y;
	int x;
	int cnt;
} T;


const int dy[] = { -1, 0, 0, 1 };
const int dx[] = { 0, -1, 1, 0 };

int N;	//격자 크기 (2~15)
int M;	//사람 수 (1 ~ min(n^2, 30))
		//베이스캠프 수 (m ~ (n^2 - m))

int answer;	//모든 사람이 편의점에 도착하는 시간

queue<Node> waitingList;	//대기열
queue<Node> q;	//돌아다니는 친구들
int LOCK[MAX][MAX];
vector<pii> REF;

bool isValid(int y, int x) {
	if (y < 0 || x < 0 || y >= N || x >= N)
		return (false);
	return (true);
}

int getDistance(int y1, int x1, int y2, int x2, const int& time) {
	int dist = -1;
	queue<T> qt;
	bool visited[MAX][MAX] = { { false }, };

	qt.push({ y1, x1, 0 });
	visited[y1][x1] = 0;
	while (!qt.empty()) {
		int y = qt.front().y;
		int x = qt.front().x;
		int cnt = qt.front().cnt;
		qt.pop();

		if (y == y2 && x == x2) {
			dist = cnt;
			break;
		}
	
		for (int flag = 0; flag < 4; ++flag) {
			int ny = y + dy[flag];
			int nx = x + dx[flag];

			if (!isValid(ny, nx))	continue;
			if (visited[ny][nx])	continue;
			if (LOCK[ny][nx] != 0 && LOCK[ny][nx] - 1 <= time)	continue;

			qt.push({ ny, nx, cnt + 1 });
			visited[ny][nx] = true;
		}
	}

	return (dist);
}

/* TODO: 격자 내 모두 타겟(편의점)으로 1칸 이동
	- 최단 거리로 움직여야 하며, 방법이 여러가지라면 상/좌/우/하 우선 순위로 움직임
	- 편의점 도착 시, queue에 넣지 않고 LOCK 걸음
*/
pii getPos(const Node& node, const int& time) {
	pii rst = make_pair(-1, -1);
	queue<T> qt;
	stack<pii> st;
	int visited[MAX][MAX] = { { 0, }, };
	int backY, backX, backCnt;
	T t;

	t.y = node.y;
	t.x = node.x;
	t.cnt = 1;

	qt.push(t);
	visited[t.y][t.x] = t.cnt;
	while (!qt.empty()) {
		int y = qt.front().y;
		int x = qt.front().x;
		int cnt = qt.front().cnt;
		qt.pop();

		if (y == node.targetY && x == node.targetX) break;
	
		for (int flag = 0; flag < 4; ++flag) {
			int ny = y + dy[flag];
			int nx = x + dx[flag];
			
			if (!isValid(ny, nx)) 	continue;
			if (LOCK[ny][nx] != 0 && LOCK[ny][nx] <= time)	continue;
			if (visited[ny][nx] != 0 && visited[ny][nx] <= cnt)	continue;

			visited[ny][nx] = cnt + 1;
			qt.push({ ny, nx, cnt + 1 });
		}
	}	

	backY = node.targetY;
	backX = node.targetX;
	backCnt = visited[backY][backX];
	st.push(make_pair(backY, backX));
	while (true) {
		for (int flag = 0; flag < 4; ++flag) {
			int ny = backY + dy[flag];
			int nx = backX + dx[flag];

			if (!isValid(ny, nx)) 	continue;
			if (visited[ny][nx] == backCnt - 1) {
				backY = ny;
				backX = nx;
				--backCnt;
				break;
			}
		}

		if (backY == node.y && backX == node.x)
			break;
		else {
			st.push({ backY, backX });
		}
	}

	rst.first = st.top().first;
	rst.second = st.top().second;

	return (rst);
}

void moveNode(const int& time) {
	int size = q.size();

	for (int i = 0; i < size; ++i) {
		Node node = q.front();
		pii pos;
		q.pop();

		pos = getPos(node, time);
		if (pos.first == -1 || pos.second == -1) {
			cout << "[moveNode] 움직일 노드 좌표 가져오는데 문제 발생\n";
			exit(1);
		}

		if (pos.first == node.targetY && pos.second == node.targetX)
			LOCK[node.targetY][node.targetX] = time + 1;
		else {
			node.y = pos.first;
			node.x = pos.second;
			q.push(node);
		}
	}
}

/* TODO: T번 사람이 가고 싶은 편의점과 가장 가까이에 있는 베이스 캠프에 들어감
	- 가장 가까운 베이스캠프가 여러가지라면 작은 행 -> 작은 열 기준으로 들어감
	- 다음 턴부터 절대 해당 베이스캠프 이동 불가
*/
pii getBasecamp(const Node& node, const int& time) {
	int std = 987654321;
	pii rst = make_pair(-1, -1);

	for (int i = 0; i < REF.size(); ++i) {
		int y = REF[i].first;
		int x = REF[i].second;
		int dist;

		if (LOCK[y][x] != 0 && LOCK[y][x] <= time)	continue;

		dist = getDistance(y, x, node.targetY, node.targetX, time);
	
		if (dist == -1)	continue;

		if (dist < std) {
			std = dist;
			rst = make_pair(y, x);
		}
		else if (dist == std) {
			if (y < rst.first) {
				std = dist;
				rst = make_pair(y, x);
			}
			else if (y == rst.first) {
				if (x < rst.second) {
					std = dist;
					rst = make_pair(y, x);
				}
			}
		}
	}

	return (rst);
}

void goBasecamp(const int& time) {
	if (waitingList.empty())	return;

	Node node = waitingList.front();
	pii basecamp;
	waitingList.pop();

	basecamp = getBasecamp(node, time);

	if (basecamp.first == -1 || basecamp.second == -1) {
		cout << "[goBaseCamp] 베이스 캠프 에러 발생\n";
		exit(1);
	}

	node.y = basecamp.first;
	node.x = basecamp.second;

	q.push(node);
	LOCK[basecamp.first][basecamp.second] = time + 1;
}

void solution() {
	int time = 0;

	while (++time) {
		moveNode(time);
		goBasecamp(time);
		if (q.empty() && waitingList.empty())	break;
	}

	answer = time;
}

void input() {
	cin >> N >> M;

	for (int y = 0; y < N; ++y)
		for (int x = 0; x < N; ++x) {
			int n;
			cin >> n;

			if (n == 1)
				REF.push_back(make_pair(y, x));
		}
			
	for (int id = 1; id <= M; ++id) {
		Node node;
		int targetY, targetX;

		cin >> targetY >> targetX;

		node.id = id;
		node.targetY = targetY - 1;
		node.targetX = targetX - 1;
		node.y = -1;
		node.x = -1;

		waitingList.push(node);
	}
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);	cout.tie(NULL);

	input();
	solution();

	cout << answer;
}