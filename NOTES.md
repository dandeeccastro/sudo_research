# Baron Samedit: Pesquisa

## LiveOverflow Playlist

- sudoedit -s 'String com \'
- Ninguém viu isso por 10 anos
- Ficou escondido porque era algo meio complicado de fazer fuzzing

- Como que alguém acharia essa brincadeira?
  - sudoedit na real é um symlink pro sudo padrão que usa argv[0] pra verificar que tipo de sudo é
  - usa uma função chamada getprogname() pra poder pegar o nome do role 
  - ainda usa o tal do setuid
    - Programas com setuid tem uid e euid (effective)
    - setuid: uid normal com euid 0
    - Só que se você tenta emular isso, você pode rodar o sudo como root, o que dá em outras funcionalidades

  - Descoberto pela equipe de pesquisa fazendo um code review

- Como que o bug funciona
  - Faz um loop por um argv previamente parseado
  - Para cada string que ele encontrar, ele pega o tamanho dela e incrementa uma variável _size_
  - Depois ele cria um array _user_args_ com malloc (o alvo)
  - Faço um loop por _user_args_ copiando caracter a caracter do texto, pulando qualquer backslash (escaped characters, normal)

  - Problema é: o que acontece se você tem uma string que termina com backslash?
    - Ele olha pro backslash no final da string, pula ela, copia o null byte de final de string e lê coisas fora argv até achar outro backslash
    - Ele copia mais dados que o array original comporta: buffer overflow

  - Mas isso não é tudo!
    - Uma função anterior procura qualquer tipo de backslash e adiciona outro backslash, pra escapar o próprio escape
    - A função vulnerável depende da função anterior, ergo o código é seguro... mais ou menos
    - Se existe um jeito de fazer a função anterior não ser chamada e a posterior sim, acabou
    - E isso existe, se chama `sudoedit -s`, que seta flags de funcionamento de forma a bypassar um e entrar no outro

  - Escalando um buffer overflow na heap
    - Diferente de um buffer overflow na stack, acessar a heap implica em tentar mudar dados que são alocados dinamicamente
    - Esses dados na heap dependem de outras funções chamadas pelo programa
    - A cada execução a heap pode mudar, a alocação não é num espaço contínuo de memória
    - Pesquisadores descobriram uma função sendo consistentemente chamada no uso do `sudoedit -s`, a `nss_lookup_function`
    - Ela usa um struct interno para poder carregar uma biblioteca externa usando `dlopen`
    - Manipulando a heap para que `user_args` ficasse na frente do struct, eles conseguem sobrescrever a struct e fazer o `dlopen` chamar a dl que eles quiserem
    - RCE com permissões de super usuário -> Priviledge Escalation -> Dedo no cu e gritaria
  
- Qual foi o fix feito pela equipe do sudo?
  - Estão verificando o que vem depois do backslash
  - Melhoraram o jeito de setar flags dentro do sudo (ISSET ao invés de &)
    - Curiosamente, o diff acusa que o ISSET é uma macro que já existia no sudo mas não tinha sido usada naquela parte do código
    - Indicador de código legado
