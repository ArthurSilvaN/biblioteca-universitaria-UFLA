/**
* Projeto Pratico - Biblioteca.
* Alunos:
 * Arthur Nunes
 * Caio Bastos
 * Pedro Rabelo
*/

#include <iostream>
#include <fstream>

#define N 3
using namespace std;

//variaveis declaradas globalmente para facilitar a manipulação e redimensionamento de usuários
int numUsuarios = 0;
int capacidadeUsuarios = 3;

ofstream arquivoSaidaBinario;
ifstream arquivoEntradaBinario;

//Todas as estruturas. Utilizamos uma de data para facilitar o uso de datas

struct Data {
    int dia;
    int mes;
    int ano;
};

struct Livro {
    char nomeLivro[50];
    Data dataDevolucao;
    bool devolvido;
};

struct Usuario {
    int numeroLivros = 0;
    int capacidadeLivros = 3;
    char nomeUsuario[50];
    char cpf[50];
    long matricula;
    char endereco[50];
    char telefone[50];
    char email[50];
    bool ativo;
    Livro *livrosEmprestados = new Livro[N];
};

//metodo para redimensionar capacidade de usuarios

void redimensionaCapacidadeUsuarios(Usuario *&usuarios) {
    int novaCapacidade = capacidadeUsuarios + 3;
    Usuario *listaAuxiliar;
    listaAuxiliar = new Usuario[novaCapacidade];

    for (int i = 0; i < capacidadeUsuarios; i++) {
        listaAuxiliar[i] = usuarios[i];
    }

    delete[] usuarios;
    usuarios = listaAuxiliar;
    capacidadeUsuarios = novaCapacidade;
}

//metodo para redimensionar capacidade de livros

void redimensionaCapacidadeLivros(Livro *&livros, int &capacidadeLivros) {
    int novaCapacidade = capacidadeLivros + 3;
    Livro *listaAuxiliar;
    listaAuxiliar = new Livro[novaCapacidade];

    for (int i = 0; i < capacidadeLivros; i++) {
        listaAuxiliar[i] = livros[i];
    }

    delete[] livros;
    livros = listaAuxiliar;
    capacidadeLivros = novaCapacidade;
}

//metodo utilizado somente para tornar usuario inativo
void excluiUsuario(Usuario *&usuario) {
    usuario->ativo = false;
}

/**
* Utilizamos dois metodos para ordenação. Matricula para facilitar a busca binaria e nome do usuario alfabeticamente.
*/

int particionaOrdenacao(Usuario *&usuarios, int comeco, int fim) {
    int i = comeco + 1, j = fim;
    Usuario pivo = usuarios[comeco];

    while (i <= j) {
        if (usuarios[i].nomeUsuario[0] <= pivo.nomeUsuario[0]) i++;
        else if (pivo.nomeUsuario[0] <= usuarios[j].nomeUsuario[0]) j--;
        else {
            swap(usuarios[i], usuarios[j]);
            i++;
            j--;
        }
    }
    usuarios[comeco] = usuarios[j];
    usuarios[j] = pivo;
    return j;
}

void ordenaUsuariosAlfabeticamente(Usuario *&usuarios, int posPivo, int fim) {
    int novaPosPivo;
    if (posPivo < fim) {
        novaPosPivo = particionaOrdenacao(usuarios, posPivo, fim);
        ordenaUsuariosAlfabeticamente(usuarios, posPivo, novaPosPivo - 1);
        ordenaUsuariosAlfabeticamente(usuarios, novaPosPivo + 1, fim);
    }
}

int particionaOrdenacaoMatricula(Usuario *&usuarios, int comeco, int fim) {
    int i = comeco + 1, j = fim;
    Usuario pivo = usuarios[comeco];

    while (i <= j) {
        if (usuarios[i].matricula <= pivo.matricula) i++;
        else if (pivo.matricula <= usuarios[j].matricula) j--;
        else {
            swap(usuarios[i], usuarios[j]);
            i++;
            j--;
        }
    }
    usuarios[comeco] = usuarios[j];
    usuarios[j] = pivo;
    return j;
}

void ordenaUsuariosMatricula(Usuario *&usuarios, int posPivo, int fim) {
    int novaPosPivo;
    if (posPivo < fim) {
        novaPosPivo = particionaOrdenacaoMatricula(usuarios, posPivo, fim);
        ordenaUsuariosMatricula(usuarios, posPivo, novaPosPivo - 1);
        ordenaUsuariosMatricula(usuarios, novaPosPivo + 1, fim);
    }
}

//metodo de busca binária
Usuario *buscaUsuario(Usuario *&usuarios, int inicio, int fim, long matricula) {
    int i = (inicio + fim) / 2;

    if (inicio > fim) {
        return nullptr;
    }

    if (usuarios[i].matricula == matricula) {
        return &usuarios[i];
    }

    if (usuarios[i].matricula < matricula) {
        return buscaUsuario(usuarios, i + 1, fim, matricula);
    } else {
        return buscaUsuario(usuarios, inicio, i - 1, matricula);
    }
}

//metodo para buscar usuario. Chamando outros dois metodos, primeiro de ordenação depois de busca QuickSort
Usuario *encontraUsuario(Usuario *&usuarios, long matricula) {
    ordenaUsuariosMatricula(usuarios, 0, numUsuarios - 1);
    return buscaUsuario(usuarios, 0, numUsuarios, matricula);
}

//metodo para "excluir" usuario (tornar o usuario inativo)
void gerenciaExclusao(Usuario *usuarios) {
    long matricula;
    int opcao;
    cout << "Digite o numero da matricula a ser excluida: ";
    cin >> matricula;
    Usuario *usuarioAtual = encontraUsuario(usuarios, matricula);

    if (usuarioAtual != nullptr) {
        if (usuarioAtual->ativo) {
            cout << "Nome do usuario: " << usuarioAtual->nomeUsuario << endl;
            cout << "Confirma exclusao? 1 - Sim || 2 - Nao" << endl;
            cin >> opcao;
            if (opcao == 1) {
                excluiUsuario(usuarioAtual);
                cout << "Nome do usuario: " << usuarioAtual->nomeUsuario << endl;
                if (usuarioAtual->ativo) {
                    cout << "Situacao: Inativo." << endl;
                }
            }
        } else {
            cout << "Matricula Inexistente" << endl;
        }
    } else {
        cout << "Matricula Inexistente" << endl;
    }
}

//metodo para facilitar o calculo de datas para fazer comparação
int transformaEmDias(Data data) {
    int diasTotais = 0;

    diasTotais += data.dia;
    diasTotais += data.mes * 30;
    diasTotais += data.ano * 365;

    return diasTotais;
}

//metodo para validar se o livro está ou não atrasado
bool atrasado(const Usuario &usuario, Data dataAtual) {
    for (int i = 0; i < usuario.numeroLivros; i++) {
        if (transformaEmDias(dataAtual) > transformaEmDias(usuario.livrosEmprestados[i].dataDevolucao)) {
            return true;
        }
    }
    return false;
}

//metodo para passar string para char para manipular arquivo binário
void *parseChar(string source, char *characters) {
    for (int i = 0; i < 50; i++) {
        characters[i] = source[i];
    }
    return nullptr;
}

//metodo para cadastrar um livro para o usuário
void realizaEmprestimo(Usuario *&usuario, char nomeLivro[50], Data dataDevolucao) {
    if (usuario->numeroLivros == usuario->capacidadeLivros) {
        redimensionaCapacidadeLivros(usuario->livrosEmprestados, usuario->capacidadeLivros);
        cout << usuario->capacidadeLivros;
    }

    usuario->livrosEmprestados[usuario->numeroLivros].dataDevolucao = dataDevolucao;
    parseChar(nomeLivro, usuario->livrosEmprestados[usuario->numeroLivros].nomeLivro);
    usuario->livrosEmprestados[usuario->numeroLivros].devolvido = false;

    usuario->numeroLivros++;
}

//metodo de gerencia de emprestimo para o usuario
void gerenciaEmprestimo(Usuario *usuarios) {
    long matricula;
    char nomeLivro[50];
    Data dataAtual, dataDevolucao;
    Usuario *usuarioAtual;

    cout << "Digite o numero da matricula: ";
    cin >> matricula;
    usuarioAtual = encontraUsuario(usuarios, matricula);
    if (usuarioAtual != nullptr) {
        if (usuarioAtual->ativo) {
            cout << "Digite o dia atual: ";
            cin >> dataAtual.dia;
            cout << "Digite o mes atual: ";
            cin >> dataAtual.mes;
            cout << "Digite o ano atual: ";
            cin >> dataAtual.ano;
            if (atrasado(*usuarioAtual, dataAtual)) {
                cout << "Usuario possui livros com devolucao atrasada! Nao e possivel realizar emprestimo." << endl;
            } else {
                cout << "Digite o nome do livro a ser emprestado: ";
                cin >> nomeLivro;
                cout << "Digite o dia da devolucao do livro: ";
                cin >> dataDevolucao.dia;
                cout << "Digite o mes de devolucao do livro: ";
                cin >> dataDevolucao.mes;
                cout << "Digite o ano de devolucao do livro: ";
                cin >> dataDevolucao.ano;
                realizaEmprestimo(usuarioAtual, nomeLivro, dataDevolucao);
                cout << "Empréstimo realizado com sucesso!" << endl;
            }
        } else {
            cout << "Matricula Inexistente" << endl;
        }
    } else {
        cout << "Matricula Inexistente" << endl;
    }
}

//metodo para gerencia de devolução
void gerenciaDevolucao(Usuario *usuarios) {
    long matricula;
    int posLivro, livrosAtivos = 0;

    cout << "Digite o numero da matricula: ";
    cin >> matricula;

    Usuario *usuarioAtual = encontraUsuario(usuarios, matricula);

    if (usuarioAtual != nullptr) {
        if (usuarioAtual->ativo) {
            cout << usuarioAtual->nomeUsuario << ", qual livro deseja devolver?" << endl;

            for (int i = 0; i < usuarioAtual->numeroLivros; i++) {
                if (!usuarioAtual->livrosEmprestados[i].devolvido) {
                    cout << "[" << i << "]" << " - " << usuarioAtual->livrosEmprestados[i].nomeLivro << endl;
                    livrosAtivos++;
                }
            }

            if (livrosAtivos > 0) {
                cout << "Digite o numero [N] ao lado do nome do livro que deseja devolver: ";
                cin >> posLivro;

                usuarioAtual->livrosEmprestados[posLivro].devolvido = true;
                cout << "Livro devolvido com sucesso!" << endl;
            } else {
                cout << "Nenhum livro para ser devolvido!" << endl;
            }
        } else {
            cout << "Matricula Inexistente" << endl;
        }
    } else {
        cout << "Matricula Inexistente" << endl;
    }
}

//metodo para listar dados a partir dos usuarios, ordenando alfabeticamente
void listarDados(Usuario *usuarios) {
    ordenaUsuariosAlfabeticamente(usuarios, 0, numUsuarios - 1);
    for (int i = 0; i < numUsuarios; i++) {
        if (usuarios[i].ativo)
            cout << usuarios[i].nomeUsuario << " - " << usuarios[i].matricula << " - " << usuarios[i].email << endl;
    }
}

//metodo utilizado para tranformar a estrutura em uma string formatada DD/MM/YYYY
string formatarData(Data data) {
    string dataFormatada;
    dataFormatada = to_string(data.dia) + "/" + to_string(data.mes) + "/" + to_string(data.ano);
    return dataFormatada;
}

//metodo para busca de usuario e listar livros
void consultaLivros(Usuario *usuarios) {
    long matricula;

    cout << "Digite o numero da matricula: ";
    cin >> matricula;

    Usuario *usuarioAtual = encontraUsuario(usuarios, matricula);

    if (usuarioAtual != nullptr) {
        if (usuarioAtual->ativo) {
            if (usuarioAtual->numeroLivros == 0) {
                cout << "Nenhum livro para ser devolvido!" << endl;
            } else {
                cout << "Livros a serem devolvidos: " << endl;
                for (int i = 0; i < usuarioAtual->numeroLivros; i++) {
                    if (!usuarioAtual->livrosEmprestados[i].devolvido) {
                        cout << "\"" << usuarioAtual->livrosEmprestados[i].nomeLivro << "\" - ";
                        cout << "Devolução prevista: " << formatarData(usuarioAtual->livrosEmprestados[i].dataDevolucao)
                             << endl;
                    }
                }
            }
        } else {
            cout << "Matricula Inexistente" << endl;
        }
    } else {
        cout << "Matricula Inexistente" << endl;
    }
}

//metodo para verificar se o usuário é existente para validar novo cadastro
bool usuarioExistente(int matricula, Usuario *&usuarios) {
    Usuario *verificaUsuario = encontraUsuario(usuarios, matricula);
    if (verificaUsuario != nullptr) {
        return true;
    }
    return false;
}

//metodo para cadastrar usuário
void cadastraUsuario(Usuario *&usuarios) {
    string nome, cpf, email, endereco;
    long matricula;

    if (numUsuarios == capacidadeUsuarios) {
        redimensionaCapacidadeUsuarios(usuarios);
    }

    cout << "Digite o nome do usuario: ";
    cin.ignore();
    getline(cin, nome);

    cout << "Digite o CPF: ";
    cin.ignore();
    getline(cin, cpf);

    cout << "Digite o numero da matricula: ";
    cin >> matricula;

    if (usuarioExistente(matricula, usuarios)) {
        cout << "Matricula já cadastrada!" << endl;
    } else {
        parseChar(nome, usuarios[numUsuarios].nomeUsuario);
        parseChar(cpf, usuarios[numUsuarios].cpf);

        usuarios[numUsuarios].matricula = matricula;

        cout << "Digite o endereco: ";
        cin.ignore();
        getline(cin, endereco);

        parseChar(email, usuarios[numUsuarios].endereco);

        cout << "Digite o telefone: ";
        cin >> usuarios[numUsuarios].telefone;

        cout << "Digite o email: ";
        cin.ignore();
        getline(cin, email);

        parseChar(email, usuarios[numUsuarios].email);

        usuarios[numUsuarios].ativo = true;

        cout << "Cadastro efetuado com sucesso!" << endl;

        numUsuarios++;
    }
}

//metodo para fazer leitura a partir do arquivo binário após exportar os dados
void validacaoArquivo() {
    Usuario *usuariosArquivo;
    usuariosArquivo = new Usuario[N];

    int i = 0;

    cout << "Dados exportados (lidos a partir do arquivo): " << endl;

    arquivoEntradaBinario.open("dadosbin.dat", ios_base::in | ios_base::binary);

    if (arquivoEntradaBinario.is_open()) {
        while (arquivoEntradaBinario.read((char *) &usuariosArquivo[i], sizeof(usuariosArquivo[i]))) {
            cout << usuariosArquivo[i].nomeUsuario << " "
                 << usuariosArquivo[i].cpf << " "
                 << usuariosArquivo[i].matricula << " "
                 << usuariosArquivo[i].telefone << " "
                 << usuariosArquivo[i].endereco << " "
                 << usuariosArquivo[i].email << " ";
            if (usuariosArquivo[i].ativo) {
                cout << "ativo" << endl;
            } else {
                cout << "inativo" << endl;
            }
            if(usuariosArquivo[i].numeroLivros > 0)
                cout << "Livros a serem devolvidos:" << endl;

            for (int j = 0; j < usuariosArquivo[i].numeroLivros; j++) {
                if (!usuariosArquivo[i].livrosEmprestados[j].devolvido) {
                    cout << "\"" << usuariosArquivo[i].livrosEmprestados[j].nomeLivro << "\" -";
                    cout << "Devolução prevista: "
                         << formatarData(usuariosArquivo[i].livrosEmprestados[j].dataDevolucao)
                         << endl;
                }
            }

            i++;
        }
    }
}

//salvando os dados em formato binário
void exportaDados(Usuario *&usuarios) {
    arquivoSaidaBinario.open("dadosbin.dat", ios_base::out | ios_base::binary);

    for (int i = 0; i < numUsuarios; i++) {
        arquivoSaidaBinario.write((char *) &usuarios[i], sizeof(usuarios[i]));
    }
    arquivoSaidaBinario.close();

    validacaoArquivo();
}

//metodo para seleção de opções dos subprogramas
void selecionaOpcoes(int arg, Usuario *&usuarios) {
    switch (arg) {
        case 1: {
            cadastraUsuario(usuarios);
            break;
        }
        case 2: {
            consultaLivros(usuarios);
            break;
        }
        case 3: {
            listarDados(usuarios);
            break;
        }
        case 4: {
            gerenciaExclusao(usuarios);
            break;
        }
        case 5: {
            gerenciaEmprestimo(usuarios);
            break;
        }
        case 6: {
            gerenciaDevolucao(usuarios);
            break;
        }
        case 7: {
            exportaDados(usuarios);
            break;
        }
        case 8: {
            cout << "Saindo...";
            break;
        }
        default:
            cout << "Operação Invalida!" << endl;
            break;
    }
}

/**
* Funcao para printar o menu de opcoes do programa.
*/
void mostraOpcoes() {
    cout << "OPTIONS: " << endl;
    cout << "[1] - Cadastrar usuário." << endl;
    cout << "[2] - Consultar livros emprestados." << endl;
    cout << "[3] - Listar usuarios." << endl;
    cout << "[4] - Excluir dados" << endl;
    cout << "[5] - Efetuar emprestimo" << endl;
    cout << "[6] - Efetuar devolucao." << endl;
    cout << "[7] - Exportar contas." << endl;
    cout << "[8] - Sair." << endl;
    cout << "Escolha uma opcao: ";
}

int main() {
    int arg;

    Usuario *usuarios;
    usuarios = new Usuario[N];

    do {
        mostraOpcoes();

        cin >> arg;

        selecionaOpcoes(arg, usuarios);
    } while (arg != 8);

    return 0;
}