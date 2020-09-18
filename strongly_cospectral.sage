from sage.graphs.trees import TreeIterator

fo = open("log24.txt", 'a')

def strongly_cospectral(X, n, cnt):
    phi_X = X.charpoly()
    phi = []
    
    # precompute the characteristic polynomial
    for a in range(n):
        X_a = (X.delete_rows([a])).delete_columns([a])
        phi.append(X_a.charpoly())   
    
    # compute set of cospectral vertices with a given vertex
    cospectral = []
    for a in range(n):
        cos = [a]
        for b in range(a+1, n):
            if(phi[a]==phi[b]):
                cos.append(b)
        if(len(cos)>2):
            cospectral.append(cos)

    # try to find strongly cospectral vertices
    for cos in cospectral:
        a = cos[0]
        scos = [a]
        for b in cos[1:]:
            X_ab = X.delete_rows([a, b]).delete_columns([a, b])
            phi_ab = X_ab.charpoly()

            g = phi_X.gcd(phi_ab)
            f = phi_X / g
            f1 = f.derivative(x)

            val = f.gcd(f1)

            if val.derivative()==0:
                scos.append(b)
                print(f"At graph {cnt}, vertices {a} and {b} are strongly cospectral")
                fo.write(f"At graph {cnt}, vertices {a} and {b} are strongly cospectral \n")
                
        if(len(scos)>2):
            print(f"3 VERTICES STRONGLY COSPECTRAL!")
            print("Graph: ")
            print(X)
            print(f"Vertices: {scos[0]}, {scos[1]}, {scos[2]}")
            fo.write(f"3 VERTICES STRONGLY COSPECTRAL!")
            fo.write("Graph: ")
            fo.write(f"{X}\n")
            fo.write(f"Vertices: {scos[0]}, {scos[1]}, {scos[2]}")


if __name__ == "__main__":

	# Change here the values
	n = 1
	r = 10

	for i in range(n, n+r):
	    print(f"Trying graphs with {i} vertices")
	    fo.write(f"Trying graphs with {i} vertices\n")
	    cnt = 0
	    for t in TreeIterator(i):
	        cnt+=1
	        A = t.adjacency_matrix()
	        
	        #P = t.plot()
	        #P.show()
	        strongly_cospectral(A, i, cnt)
