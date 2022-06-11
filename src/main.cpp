#include <ios>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>



/* Problema do ordenamento dos times de forma que o time à esquerda na lista venceu o sime seguinte à sua direita.
 * Este problema assume a competição em um jogo onde não ocorrem empates e cada time jogou contra todos os outros times.
 *
 * Entrada do programa:
 *
 *    - arg[1]:   Deve ser o caminho para o arquivo com a tabela/lista de vitórias.
 *                   A primeira linha deve conter o número de times.
 *                   As linhas seguintes correspondem cada uma a uma partida. São dois números separados por um espaço, em cada linha.
 *                   Cada número representa o respectivo time. O primeiro número, à esquerda, foi o vitorioso da partida.
 *
 *    - arg[2]:   Deve ser o caminho para o arquivo com as sequências/instâncias de entrada.
 *                   Os times devem ser numerados de 0 a 'n' e separados por espaço.
 *                   As instâncias devem estar separadas por quebras de linha.
 *
 * Saída do programa:
 *
 *    Um comjunto de números representando os respectivos times, em uma lista ordenada tal que o time à esquerda venceu o time à direita.
*/



std::vector<int> orderTeams(const std::vector<int> teams, const int _next, const std::vector<int> solution, int** victory_matrix) {

   /* DEBUG: Para acompanhar os passos.
   std::cout << "\nTeams: ";
   for (int val : teams) std::cout << " " << val;
   std::cout << "\nNext: " << _next << '\n';
   std::cout << "Solution: ";
   for (int val : solution) std::cout << " " << val;
   std::cout << '\n'; */

   // solução_nova = vetor []
   std::vector<int> new_solution;

   // Caso especial de inicialização do algoritmo. Como 0 é um time válido, usamos -1 como raiz da árvore.
   int next;
   if (_next == -1) {
      for (int i=0; i < teams.size(); i++) {
         std::vector<int> teams_left = teams;
         teams_left.erase(teams_left.begin()+i);
         new_solution = orderTeams(teams_left, teams[i], solution, victory_matrix);
         if (!new_solution.empty()) return new_solution;
      }
   }

   // Se esta iteração não for -1, então já descemos da raiz e a inicialização já foi feita.
   next = _next;

   // se times.tam == 0 e solução.tam > 0 e vitorias[solução[-1]][prox]:
   if (teams.empty() && !solution.empty() && victory_matrix[solution.back()][next] == solution.back()) {
      // retorne solução + [prox]
      new_solution = solution;
      new_solution.push_back(next);
   }

   // senão se times.tam == 1 e solução.tam == 0: retorne times
   else if (teams.size() == 1 && solution.empty()) return teams;

   // senão se solução.tam == 0 ou vitorias[solução[-1]][prox] == solução[-1]:
   else if (solution.empty() || victory_matrix[solution.back()][next] == solution.back()) {

      // Tem que criar a nova solução atual aqui, solução += [prox]
      std::vector<int> curr_solution = solution;
      curr_solution.push_back(next);

      // para i = 0 -> times.tam:
      for (int i=0; i < teams.size(); i++) {

         // times_reman = times[:i] + times[i+1:]
         std::vector<int> teams_left = teams;
         teams_left.erase(teams_left.begin()+i);  // Esquisitice de iteradores do C++ ...

         // solução_nova = ordenarTimes(times_reman, times[i], solução += [prox], vitorias)
         new_solution = orderTeams(teams_left, teams[i], curr_solution, victory_matrix);

         // se solução_nova.tam > 0: quebre
         if (!new_solution.empty()) break;
      }
   }

   return new_solution;
}



int main(int argc, char* argv[]) {


   // #1 CARREGAMENTO DA ENTRADA

   const int BUFFER_LENGTH = 128;      // Tamanho do buffer de leitura das linhas dos arquivos de entrada.
   char buffer[BUFFER_LENGTH];         // Buffer de leitura mencionado.
   std::ifstream file_sequences;       // Input File Stream para abrir o arquivo de entrada com as possíveis sequências de entrada.
   std::ifstream file_victory_tab;     // Input File Stream para abrir o arquivo de entrada com a tabela/lista de vitórias.

   // DEBUG: Exibe a lista de argumentos recebidos na execução do programa.
   //for (int i=1; i < argc; i++) std::cout << argv[i] << " ";
   //std::cout << '\n';

   // Se temos os argumentos necessários, abrimos os arquivos.
   if (argc > 2){
      file_victory_tab = std::ifstream(argv[1], std::ios::binary);
      file_sequences = std::ifstream(argv[2], std::ios::binary);
   }
   else {
      std::cout << "Argumentos insificientes para execução.\n" << "Uso:\n\tbin/q_3_2 tabela_de_vitorias.txt arquivo_de_sequencias.txt\n";
      return 1;
   }

   // Encerra se houve erro ao abrir arquivo com a lista de vitórias.
   if (!file_victory_tab.is_open()) {
      std::cout << "Erro ao abrir arquivo " << argv[1] << '\n';
      return 1;
   }

   // Encerra se houve erro ao abrir arquivo com as sequências.
   if (!file_sequences.is_open()) {
      std::cout << "Erro ao abrir arquivo " << argv[2] << '\n';
      return 1;
   }


   // #2 LEITURA DA INSTÂNCIA

   // Prepara array de vitórias.
   file_victory_tab.getline(buffer, BUFFER_LENGTH);

   //DEBUG: Exibe quantidade de times descrito no arquivo que lista as vitórias
   //std::cout << "Times: " << buffer << '\n';

   int size = atoi(buffer);              // Tamanho da matriz de vitórias.
   int** victories = new int* [size];    // Prepara base da matriz de vitórias. `int victories[size][size];` não passa como argumento.
   for (int y=0; y < size; y++)
      victories[y] = new int[size];      // Alloca as linhas.
   for (int i=0; i < size; i++)
      victories[i][i] = -1;              // Mete -1 na diagonal principal porque foda-se, um time não joga contra si mesmo.
   std::string substr_win, substr_lose;  // Buffer pra ir lendo os times a cada linha lida.
   int winner, loser;                    // Para cada partida sendo interpretada, recebem os números dos times.

   /* Todos as classes de stream descendem de std::basic_ios e por isso tem acesso ao método std::basic_ios::fail().
    * Quando std::ifstream::getline() tenta ler além do final de um arquivo aberto, ele chama std::basic_ios::setstate(std::basic_ios::eofbit).
    * Isso define o bit de estado que marca se o final do arquivo aberto no ifstream foi atingido.
    * E quando o bit de estado do EOF está definido, std::basic_ios::fail() retorna true. */
   while(!file_victory_tab.fail()) {                     // Enquanto EOF não for atingido, execute:
      file_victory_tab.getline(buffer, BUFFER_LENGTH);   // Carregue a próxima linha para o buffer.
      //std::cout << buffer << '\n';                     // DEBUG: Exibe o que foi lido a cada iteração.
      std::stringstream buffer_str_stream(buffer);       // Cria um String Stream com a string do buffer.

      while(buffer_str_stream.good()) {                        // Isso aqui é equivalente a !std::basic_ios::fail().
         try {
            std::getline(buffer_str_stream, substr_win, ' ');     // Funciona como std::bsicc_ios::getline() com delimitador ' '. Escreve no segundo argumento.
            std::getline(buffer_str_stream, substr_lose, '\n');   // O segundo time, perdedor, é seguido por caracter de nova linha.
            winner = stoi(substr_win);                            // Cast em int.
            loser = stoi(substr_lose);                            // Cast em int.
            victories[winner][loser] = winner;     // Atualiza matriz de vitórias com quem venceu a partida entre esses dois times.
            victories[loser][winner] = winner;     // Mesma coisa para o triângulo superior da matriz. Caso a gente precise acessar por essa parte da matriz.
         }

         /* As bostas dos getline() ainda me dão uma string vazia na última iteração do laço, antes de sair por causa do EOF, bem como std::string vazias
          * nas respectivas leituras das substrings. Daí quando chega nos stoi(substr) dá exceção std::invalid_argument e fode tudo.
          * Talvez haja solução melhor que um try-catch com a porra de um break, que ocorre exatamente na última iteração do laço, mas tome no cu. */
         catch (const std::invalid_argument& inv_arg_exc) {
            break;
         }
      }
   }

   // INFO: Exibe matriz de vitórias lida.
   std::cout << "Matriz de vitórias:\n";
   for (int j=0; j < size; j++) {
      for (int i=0; i < size; i++) {
         std::cout << ' ' << victories[i][j];
      }
      std::cout << '\n';
   }


   // #3 PROCESSAMENTO DAS INSTÂNCIAS

   // String pra segurar a sequência sendo interpretada.
   std::string str_seq;

   // Laço para ordenar todas as entradas fornecidas pelo arquivo de sequências fornecido.
   while(file_sequences.good()) {

      file_sequences.getline(buffer, BUFFER_LENGTH);
      str_seq = std::string(buffer);                       // Converte em string C++ pra ter acesso rápido à funçãop de comprimento da string.
      if (str_seq.length() < 1) break;                     // Pra num precisar da bixiga de um try-catch por causa da string vazia na última iteração do laço.
      //std::cout << "Ordenando " << str_seq << " ...\n";  // DEBUG: Exibe o que vai ser interpretado.

      std::vector<int> teams;                    // Vetor para colocar os times na ordem de leitura da sequência atual da instância.
      std::vector<int> solution;                 // Vetor para receber a solução.
      std::stringstream curr_seq_line(str_seq);  // String Stream pra ir lendo os times da linha que descreve a sequência atual.
      std::string substr;                        // Pra agarrar de dentro do buffer_str_stream a substring com o número do time.

      // Pega os times na ordem recebida. Lembrando que 'size' é a quantidade de times (tamanho da entrada).
      for(int i=0; i < size; i++) {
         std::getline(curr_seq_line, substr, ' ');
         teams.push_back(stoi(substr));
      }

      /* ALGORITMO: Invoca algoritmo central para encontrar a solução.
       * O segundo argumento (next) com -1 marca a chamada inicial do algoritmo, já que 0 é um time válido. */
      solution = orderTeams(teams, -1, solution, victories);

      // INFO: Entrada entendida.
      std::cout << "Entrada: [";
      for (int val : teams) std::cout << " " << val;
      std::cout << " ]\n";

      // INFO: Deixa eu ver essa merda.
      std::cout << "Resultado: [";
      for (int val : solution) std::cout << " " << val;
      std::cout << " ]\n";

   }

   return 0;
}
