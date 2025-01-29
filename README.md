## Command line client for llama.cpp

### Video tutorial:
https://www.youtube.com/watch?v=rsBnAF5ZLK8

### Usage instructions:
1. Requirements:
   The tool requires the libraries and headers (-dev packages)
   of curl, json-c and gnu readline. On debian based systems, i.e.
   debian, ubuntu etc. an install of these can be achieved using:
   ```
   sudo apt-get install build-essential libjson-c-dev libcurl-dev libreadline-dev  
   ```
   Further you need a llama.cpp compatible large language and embedding model.
   For the following instructins we suggest: 

   https://huggingface.co/lmstudio-community/Meta-Llama-3.1-8B-Instruct-GGUF

   https://huggingface.co/nomic-ai/nomic-embed-text-v1.5-GGUF

2. Build:
   in most cases a simple `make all` should be enough.
   In case it does not work edit the makefile in order to
   satisfy your systems libraries cflags.

3. Conversation Run:
   
   3.1 Start a llama.cpp server:
      ```
      llama.cpp/bin/llama-server -m Meta-Llama-3.1-8B-Instruct-Q6_K.gguf --host 127.0.0.1
      ```
   3.2 Connect to your llama.cpp server with the client:
       ```
       bin/rag-conversation 127.0.0.1 8080 -1
       ```
       When you type your text finish with `Ctrl-d`. This allows multiline input
       on the terminal. 

4. Run with RAG:
   
   4.1 Start a llama.cpp server to generate embeddings:
   ```
   llama.cpp/bin/llama-server -m nomic-embed-text-v1.5.f16.gguf --host localhost --port 8081
   ```
   4.2 Create a vector database from a text document:
   ```
   bin/build-vector-db-from-server your-text.txt localhost 8081 2000 your-text.vdb
   ```
   4.3 Run vector database supported and talk about your text:
   ```
   bin/rag-with-vdb-cos-client localhost 8080 -1 your-text.vdb 3 localhost 8081
   ```
