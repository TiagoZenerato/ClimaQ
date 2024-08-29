# Guia para Abrir e Compilar o Projeto `ClimaQ` no VSCode

Este guia irá orientá-lo sobre como abrir o projeto `ClimaQ` no Visual Studio Code (VSCode), configurar a extensão ESP-IDF e compilar o projeto usando o SDK 5.1.4.

## Requisitos

Antes de começar, certifique-se de que você tem os seguintes requisitos instalados:

1. **Visual Studio Code (VSCode)** - [Download e instalação do VSCode](https://code.visualstudio.com/)
2. **ESP-IDF Extension for VSCode** - Esta extensão facilita o desenvolvimento com o ESP-IDF.
3. **ESP-IDF v5.1.4** - A versão específica do ESP-IDF para o seu projeto.

## Passo 1: Instalar o Visual Studio Code

1. Baixe e instale o VSCode a partir do [site oficial](https://code.visualstudio.com/).

## Passo 2: Instalar a Extensão ESP-IDF para VSCode

1. Abra o VSCode.
2. Vá para a aba de **Extensões** (ícone de quadrado no lado esquerdo).
3. Procure por "ESP-IDF" na caixa de pesquisa.
4. Clique em **Instalar** na extensão "Espressif IDF".

## Passo 3: Configurar a Extensão ESP-IDF

1. Após a instalação da extensão, você precisará configurar o ESP-IDF no VSCode.
2. Abra o **Command Palette** pressionando `Ctrl+Shift+P`.
3. Digite e selecione **ESP-IDF: Configure ESP-IDF**.
4. Siga as instruções para configurar o ESP-IDF. Quando solicitado, selecione o diretório onde você instalou o ESP-IDF. Para esta configuração, escolha a versão 5.1.4.

## Passo 4: Abrir o Projeto `ClimaQ` no VSCode

1. No VSCode, clique em **File** > **Open Folder**.
2. Navegue até o diretório do seu projeto `ClimaQ`. No seu caso, é `D:\GitHub\ClimaQ`.
3. Selecione a pasta `ClimaQ` e clique em **Selecionar Pasta**.

## Passo 5: Configurar o Projeto para o SDK 5.1.4

1. Certifique-se de que o ambiente ESP-IDF está configurado para usar a versão 5.1.4.
2. Abra o terminal integrado no VSCode (`Ctrl+``).
3. Navegue até o diretório do projeto `ClimaQ`.
4. Execute o comando para garantir que o ESP-IDF está configurado corretamente:
   ```bash
   idf.py --version
   ```
   Verifique se a versão é a 5.1.4.

## Passo 6: Configurar as Variáveis de Ambiente

1. No terminal integrado do VSCode, execute o comando para configurar as variáveis de ambiente:
   ```bash
   idf.py set-target esp32s3
   ```
2. Verifique se a configuração foi aplicada corretamente:
   ```bash
   idf.py menuconfig
   ```
   Isso abrirá a interface de configuração do ESP-IDF, onde você pode ajustar as configurações do projeto conforme necessário.

## Passo 7: Compilar o Projeto

1. No terminal integrado, dentro do diretório do projeto `ClimaQ`, execute o comando de build:
   ```bash
   idf.py build
   ```
2. Acompanhe a saída para verificar se a compilação foi bem-sucedida.

## Passo 8: (Opcional) Flash e Monitoramento

1. Para fazer o flash do firmware no ESP32, conecte seu dispositivo e execute:
   ```bash
   idf.py flash
   ```
2. Para monitorar a saída serial do dispositivo, execute:
   ```bash
   idf.py monitor
   ```

   Você pode usar `Ctrl+]` para sair do monitoramento.

# Observações para o teste: 
 - O projeto está configurado para minha rede! Não deixe de trocar o ssid e password (Na linha 19 e 20 do arquivo WIFIctrl)
 - No HiveMQ deve-se:
   - Inscrever no tópico climaQ/report.
   - Publicar no climaQ/TrocarCor (Se a mensagem enviada for 1 ele trocara a cor, se for 0 Ele irá desligar o led).

---

Siga estes passos e você deve conseguir abrir, configurar e compilar seu projeto `ClimaQ` no VSCode usando o ESP-IDF v5.1.4. Se encontrar algum problema, sinta-se à vontade para me chamar!