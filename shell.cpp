#include "fcntl.h"
#include "sys/wait.h"
#include "vector"
#include <cstdlib>
#include <iostream>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <vector>
using namespace std;

void print(string &s) { write(STDOUT_FILENO, s.c_str(), s.size()); }

string pre(char buffer[], int red) {
  string st = "";
  for (int i = 0; i < red - 1; i++) {
    st.push_back(buffer[i]);
  }
  return st;
}

void split(vector<string> &s, char buffer[], int red, string done) {
  string st = done;
  string temp = "";
  for (char it : st) {
    if (it != ' ' && it != '\t') {
      temp.push_back(it);
    } else {
      if (temp == "") {
        continue;
      }
      s.push_back(temp);
      temp = "";
    }
  }
  s.push_back(temp);
}

void signalHandler(int signum) {
  if (signum == SIGINT) {
    string s = "INTERRUPT SIGNAL _STDINT_ RECEIVED\n";
    print(s);
  }
  if (signum == SIGTSTP) {
    exit(signum);
  }
  if (signum == SIGCHLD) {
    string s = "CHILD GOT TERMINATED\n";
    print(s);
  }
}

void cmd(vector<string> &s, char buff[]) {
  if (s[0] == "pwd") {
    getcwd(buff, 1000);
    cout << buff << endl;
  } else if (s[0] == "exit") {
    string p = "Shell is going to exit\n";
    print(p);
    exit(0);
  } else if (s[0] == "cd") {
    if (s.size() == 1) {
      string p = "/home/";
      int c = chdir(p.c_str());
      return;
    }
    int c = chdir(s[1].c_str());
    if (c == -1) {
      string p = "Please provide a valid path\n";
      print(p);
    }
  }
}

void file(vector<string> &s) {
  int sz = s.size() - 1;
  int status;
  if (sz < 1)
    return;
  else {
    int pid = fork();
    if(pid<0){
      string s="failed in creating child";
      print(s);
      return;
    }
    if (pid == 0) {
      if (s[sz - 1] == ">>>") {
        int fd = open(s[sz].c_str(), O_WRONLY | O_CREAT, 0777);
        dup2(fd, 1);
        close(fd);
        int e = execlp(s[sz - 2].c_str(), s[sz - 2].c_str(), NULL);
        if (e == -1) {
          string s = "failed at execlp function";
          print(s);
          exit(1);
        }
        exit(0);
      } else if (s[sz - 1] == "<<<" && (sz - 1) == 1) {
        int fd = open(s[sz].c_str(), O_RDONLY | O_CREAT, 0777);
        dup2(fd, 0);
        close(fd);
        int e = execlp(s[sz - 2].c_str(), s[sz - 2].c_str(), NULL);
        if (e == -1) {
          string s = "failed at execlp function";
          print(s);
          exit(1);
        }
        exit(0);
      } else if (s[sz - 1] == "&>>") {
        int fd = open(s[sz].c_str(), O_WRONLY | O_CREAT, 0777);
        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        int e = execlp(s[sz - 2].c_str(), s[sz - 2].c_str(), NULL);
        if (e == -1) {
          string s = "failed at execlp function";
          print(s);
          exit(1);
        }
        exit(0);
      }
    } else {
      waitpid(-1, &status, 0);
    }
  }
}

void mpip(vector<string> &s) {
  int sz = s.size();
  int total = 0;
  int k = 1;
  while (k < sz && s[k] == "||") {
    total++;
    k += 2;
  }
  total++;
  int fd[total][2];
  int t = 0;
  for (int i = 0; i < total; i++) {
    if (i != total - 1) {
      if (pipe(fd[i]) < 0) {
        string s = "pipe error";
        print(s);
      }
    }
    int pid = fork();
    if (pid == -1) {
      string s = "failed to create 1st child";
      print(s);
    }
    if (pid == 0) {
      if (i != total - 1) {
        dup2(fd[i][1], STDOUT_FILENO);
        close(fd[i][0]);
        close(fd[i][1]);
      }

      if (i != 0) {
        dup2(fd[i - 1][0], STDIN_FILENO);
        close(fd[i - 1][1]);
        close(fd[i - 1][0]);
      }
      int e = execlp(s[i * 2].c_str(), s[i * 2].c_str(), NULL);
      if (e == -1) {
        string s = "failed at execlp function";
        print(s);
        exit(1);
      }
    }
    if (i != 0) {
      close(fd[i - 1][1]);
      close(fd[i - 1][0]);
    }
    wait(NULL);
  }
}

void fore(vector<string> &s, char buffer[], string done) {
  int sz = s.size() - 1;
  int status;
  string st = s[0];
  // cout << st << endl;
  if (st == "pwd" || st == "cd" || st == "exit") {
    cmd(s, buffer);
    return;
  } else if (sz > 1) {
    if ((s[sz - 1] == ">>>" || s[sz - 1] == "<<<" || s[sz - 1] == "&>>"))
      file(s);
    else {
      mpip(s);
    }
  } else {
    int status;
    int pid = fork();
    string temp = done;
    if (pid == 0) {
      int e = execlp(temp.c_str(), temp.c_str(), (char *)NULL);
      if (e == -1) {
        string p = "Failed at execlp function\n";
        print(p);
      }
      exit(0);
    } else {
      wait(0);
    }
  }
}

void back(vector<string> &s, char buffer[], string done) {
  int pid = fork();
  if (pid == 0) {
    fore(s, buffer, done);
    exit(0);
  }
}

int main() {
  signal(SIGINT, signalHandler);
  signal(SIGTSTP, signalHandler);
  signal(SIGCHLD, signalHandler);
  cout << R"(
   __ _           _
  / _| |         | |
 | |_| | __ _ ___| |__
 |  _| |/ _` / __| '_ \
 | | | | (_| \__ \ | | |
 |_| |_|\__,_|___/_| |_|

  )";
  while (1) {
    char buff[1000];
    char in[1000];
    const char msg[] = "===>";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    int cr = read(STDIN_FILENO, in, 1000);

    if (cr == 1)
      continue;

    string full = pre(in, cr);
    vector<string> s;

    int filter = 0;
    if (full.back() == '&') {
      full.pop_back();
      full.pop_back();
      split(s, in, cr, full);
      filter = 1;
    } else {
      split(s, in, cr, full);
    }

    // for (auto it : s) {
    //   cout << it << endl;
    // }

    if (filter == 1) {
      back(s, buff, full);
    } else {
      fore(s, buff, full);
    }
  }
  return 0;
}