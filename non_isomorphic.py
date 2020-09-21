from sage.graphs.trees import TreeIterator
import argparse

parser = argparse.ArgumentParser(description="""""")

parser.add_argument("-n", dest="n", type=int, help="Number of Vertices to start the search")

parser.add_argument("-r", dest="r", type=int, help="Radius of the search (from n to n+r-1)")

parser.add_argument("-s", dest="s", type=int, help="Start the search from the s'th graph")

args = parser.parse_args()

def isomorphic_test(X, n, cnt):
    phi_X = X.charpoly()
    phi = []
    matrices = []
    
    # precompute the characteristic polynomial
    for a in range(n):
        X_a = (X.delete_rows([a])).delete_columns([a])
        phi.append(X_a.charpoly())   
        matrices.append(X_a);
            
    # compute set of cospectral vertices with a given vertex
    for a in range(n):
        for b in range(a+1, n):
            if(phi[a]!=phi[b]): continue  # a and b are not cospectral 
                
            X_ab = X.delete_rows([a, b]).delete_columns([a, b])
            phi_ab = X_ab.charpoly()

            g = phi_X.gcd(phi_ab)
            f = phi_X / g
            f1 = f.derivative(x)

            val = f.gcd(f1)

            if val.derivative() != 0: continue  # a and b are not parallel
            
            G_a = Graph(matrices[a])
            G_b = Graph(matrices[b])
            
            if(G_a.is_isomorphic(G_b)): continue  # G\a ~ G\b
                
            print(f"At graph {cnt}, G\\{a} is not isomorphic to G\\{b}")
            
            with open(f"data/non_isomorphic/log_niso_{n}v.txt", "a") as fin:
                fin.write(f"{cnt}, ")           
                
n = args.n
r = args.r
for i in range(n, n+r):
    print(f"Trying graphs with {i} vertices")
    cnt = 0
    for t in TreeIterator(i):
        cnt+=1
        if(cnt<args.s): continue
        A = t.adjacency_matrix()
        
        #P = t.plot()
        #P.show()
        isomorphic_test(A, i, cnt)