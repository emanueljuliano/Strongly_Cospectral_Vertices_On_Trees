#include<bits/stdc++.h>

using namespace std;

typedef vector<vector<int>> graphT;
typedef vector<int> cycleT;
typedef pair<int, int> egT;

graphT read_graph() {
	int n, m; cin >> n >> m;
	graphT g(n);
	
	for (int i = 0; i < m; i++) {
		int a, b; cin >> a >> b; a--, b--;
		g[a].push_back(b), g[b].push_back(a);
	}

	return g;
}

vector<egT> get_edges(graphT &G) {
	vector<egT> edges;
	int n = G.size();
	for (int i = 0; i < n; i++) for (int j : G[i]) if (i < j)
		edges.emplace_back(i, j);
	return edges;
}

vector<cycleT> enumerate_even_cycles(graphT &G) {
	vector<cycleT> cycles;
	int n = G.size();
	vector<int> vis(n);
	
	function<void(int, cycleT&)> dfs = [&](int i, cycleT &c) {
		vis[i] = 1;
		c.push_back(i);

		for (int j : G[i]) {
			// close cycle and is lexmin
			if (j == c[0] and c.size() >= 6 and c.size() % 2 == 0 and i > c[1]) { 
				cycles.push_back(c);
			}
		}
		
		for (int j : G[i]) if (!vis[j]) {
			dfs(j, c);
		}
		
		c.pop_back();
		vis[i] = 0;
		return;
	};

	for (int i = 0; i < n; i++) {
		cycleT c;
		dfs(i, c);
		vis[i] = 1;
	}

	return cycles;
}

egT fix(int a, int b) {
	if (a > b) swap(a, b);
	return pair(a, b);
}

bool valid_color(egT e, graphT &G, map<egT, int> &coloring) {
	auto [a, b] = e;
	int c = coloring[e];

	// cherys
	for (int i : G[a]) if (i != b and coloring[fix(a, i)] == c) return false;
	for (int i : G[b]) if (i != a and coloring[fix(b, i)] == c) return false;

	// bicromatic C4
	for (int i : G[a]) if (i != b) {
		int c_ai = coloring[fix(a, i)];
		if (!c_ai) continue;
		for (int j : G[b]) if (j != a and j != i) {
			int c_bj = coloring[fix(b, j)];
			if (!c_bj) continue;
			int c_ij = coloring[fix(i, j)];
			if (!c_ij) continue;

			if (c_ai == c_bj and c == c_ij) return false;
		}
	}

	return true;
};

map<egT, int> greedy_coloring(graphT &G, int K, vector<egT> edges) {
	map<egT, int> coloring;
	
	for (egT e : edges) {
		do {
			coloring[e] = rand() % K + 1;
		} while(!valid_color(e, G, coloring));
	}

	return coloring;
}

vector<cycleT> bicromatic_cycles (vector<cycleT> &cycles, map<egT, int> &coloring) {
	vector<cycleT> ret;
	for (auto c : cycles) {
		set<int> colors;
		for (int i = 0; i < c.size(); i++) {
			int a = c[i], b = c[(i+1)%c.size()];
			colors.insert(coloring[fix(a, b)]);
		}
		if (colors.size() <= 2) ret.push_back(c);
	}
	return ret;
}

void run_experiment(graphT &G, int K, vector<cycleT> &cycles, const int IT = 1000000) {
	cout << "ITERATIONS: " << IT << endl;
	map<cycleT, int> bicr_occ;
	for (int it = 0; it < IT; it++) {
		if (it%(IT/10) == 0) cout << it << endl;
		vector<egT> edges = get_edges(G);
		random_shuffle(edges.begin(), edges.end());
		map<egT, int> coloring = greedy_coloring(G, K, edges);
		vector<cycleT> occurences = bicromatic_cycles(cycles, coloring);
		for (cycleT c : occurences) bicr_occ[c]++;
	}
	
	for (cycleT c : cycles) if (c.size() >= 6) {
		cout << "cicle: ";
		for (int i : c) cout << i+1 << " ";
		cout << "occ: " << bicr_occ[c] << endl;
	}
}

pair<int, map<cycleT, long double>> bicromatic_occ_given_order(
	graphT &G, int K, vector<cycleT> &cycles, vector<egT> &edges
) {
	map<egT, int> coloring;
	int cnt = 0;
	map<cycleT, long double> bicr_occ;

	function<void(int, long double)> backtrack_colors = [&](int e_idx, long double p) {
		if (e_idx == edges.size()) {
			cnt++;
			vector<cycleT> occurences = bicromatic_cycles(cycles, coloring);
			for (cycleT c : occurences) bicr_occ[c] += p;
			return;
		}
		
		egT e = edges[e_idx];
		
		int free = 0;
		for (int k = 1; k <= (e_idx == 0 ? 1 : K); k++) {
			coloring[e] = k;
			if (valid_color(e, G, coloring)) free++;
		}

		for (int k = 1; k <= (e_idx == 0 ? 1 : K); k++) {
			coloring[e] = k;
			if (valid_color(e, G, coloring)) backtrack_colors(e_idx+1, p/free);
		}

		coloring[e] = 0;
	};
	
	backtrack_colors(0, 1.0);

	return pair(cnt, bicr_occ);
}

void exact_experiment(graphT &G, int K, vector<cycleT> &cycles) {
	vector<egT> edges = get_edges(G);
	sort(edges.begin(), edges.end());
	int order = 1;
	pair<long double, vector<egT>> mini(1, {}), maxi(0, {});
	long double sum = 0;
	do {
		if (order%1000 == 0)
			cout << "order: " << order++ << endl;
		else order++;
		auto [cnt, bicr_occ] = bicromatic_occ_given_order(G, K, cycles, edges);
		for (auto [c, occ] : bicr_occ) {
			//for (int i : c) cout << i+1 << " ";
			//cout << ": " << occ << " probability " << endl;
			mini = min(mini, pair(occ, edges));
			maxi = max(maxi, pair(occ, edges));
			sum += occ;
		}	
	} while(next_permutation(edges.begin(), edges.end()));
	
	cout << "Min probability : " << mini.first << ", order: ";
	for (auto [a, b] : mini.second) cout << "(" << a << "," << b << ") ";
	cout << endl;
	cout << "Max probability : " << maxi.first << ", order: ";
	for (auto [a, b] : maxi.second) cout << "(" << a << "," << b << ") ";
	cout << endl;
	cout << "Mean: " << sum/(order-1) << endl;
}

int main(int argc, char**argv) {
	srand(atoi(argv[1]));

	int K; cin >> K;
	graphT G = read_graph();
	
	vector<cycleT> cycles = enumerate_even_cycles(G);
	
	// exact_experiment(G, K, cycles);
	run_experiment(G, K, cycles);

	// Sanity check
	cout << endl << "Example of coloring" << endl;
	vector<egT> edges = get_edges(G);
	random_shuffle(edges.begin(), edges.end());
	map<egT, int> coloring = greedy_coloring(G, K, edges);
	for (auto c : cycles) {
		cout << endl << "cycle: ";
		for (int i : c) cout << i+1 << " ";
		cout << endl;
		for (int i = 0; i < c.size(); i++) {
			int a = c[i], b = c[(i+1)%c.size()];
			cout << a+1 << "-" << b+1 << ": " << coloring[fix(a, b)] << endl;
		}
	} 

	return 0;
}
