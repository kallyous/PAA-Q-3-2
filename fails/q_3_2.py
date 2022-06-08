import sys

def print_table(M):
    lines = len(M)
    for line in range(lines):
        print(M[line])

def read_victory_table(file_path):
    with open(file_path, 'r') as file:
        team_count = int(file.readline())
        M = [ [-1 for i in range(team_count)] for j in range(team_count)]
        for line in file.readlines():
            (team_a, team_b, winner) = line.split()
            team_a = int(team_a)
            team_b = int(team_b)
            winner = int(winner)
            M[team_a][team_b] = winner
            M[team_b][team_a] = winner
        return M


def read_test_sequences(file_path):
    seqs = []
    with open(file_path, 'r') as file:
        for line in file.readlines():
            seqs.append([ int(a) for a in line.split() ])
    return seqs


def build_sol(seq, vict_tab):
    sol = [seq[0]]
    for i in range(1, len(seq)):

        # Entre sol[0] e [seq[i]], se quem venceu foi seq[i]:
        if vict_tab[sol[0]][seq[i]] == seq[i]:
            # Ponha seq[i] no começo de sol, à esquerda de sol[0].
            sol = [seq[i]] + sol

        # Mas se entre sol[-1] e seq[i], quem venceu foi sol[-1]:
        elif vict_tab[sol[-1]][seq[i]] == sol[-1]:
            # Ponha seq[i] no final de sol, à direita de sol[-1].
            sol = sol + [seq[i]]

        # Caso contrário:
        else:
            # Rastreia sucesso.
            ok = False

            # Encontre um par 'a' vence 'b' tal que seq[i] perde pra 'a' e seq[i] vence 'b':
            for j in range(len(sol)):
                if vict_tab[j][seq[i]] == j and vict_tab[j+1][seq[i]] == seq[i]:

                    # Ponha seq[i] entre 'a' e 'b'.
                    sol = sol[:j+1] + [seq[i]] + sol[j+1:]

                    # Rastreia sucesso.
                    ok = True

                    break

            # Se chegamos ao final e não temos 'ok', deu merda.
            if not ok:
                print("\nFUDEU!")
                print(f"sol: {sol}")
                print(f"seq: {seq}")
                print(f"i: {i}")
                print(f"seq[i]: {seq[i]}")
                print(f"j: {j}  |  seq: {seq}  | sol: {sol}")
                print_table(vict_tab)
                print()

    return sol


def weird_sort(A, ini):
    for i in range(ini, len(A)):
        print(f"{i} : {A[i]}")


if __name__=="__main__":

    if len(sys.argv) > 2:

        vict_tab = read_victory_table(sys.argv[1])
        print("\nTabela de vitórias:")
        print_table(vict_tab)

        test_seqs = read_test_sequences(sys.argv[2])
        print("\nSequências de testes:")
        print_table(test_seqs)

        print("\nResultados:")
        for i in range(len(test_seqs)):
            solution = build_sol(test_seqs[i], vict_tab)
            print(f"{i}:\t{solution}")

    else:
        print("Forneça os caminhos para 1. tabela de vitórias e 2. sequências para testes.")
        exit()

    print()

