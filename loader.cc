#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "graph.h"

using std::string;
using std::vector;
using std::istream;
using std::istringstream;
using std::cout;
using std::cerr;
using std::endl;

class NoMoreLinesException : public std::runtime_error
{
public:
  NoMoreLinesException (const string &msg) :
      runtime_error (msg)
  {
  }
};

string
mygetline (istream &input)
{
  bool line_read = false;
  string line;
  while (!line_read)
    {
      if (input.eof ())
	throw NoMoreLinesException ("Failed to read a line from the input.");
      std::getline (input, line);
      if (!input.eof () && input.fail ())
	throw std::runtime_error (
	    "Failure occurred while trying to read a line.");
      line = line.substr (0, line.find_first_of ("\n\r#"));
      if (line.find_first_not_of (" \t") != line.npos)
	line_read = true;
    }
//   cout << line << endl;
  return line;
}

int
mygetnumber (istream &input)
{
  string line = mygetline (input);
  istringstream istr (line);
  int result;
  istr >> result;
  if (istr.fail ())
    throw std::runtime_error ("Failed to parse " + line + " as a string.");
  return result;
}

Edge
mygetedge (istream &input)
{
  string line = mygetline (input);
  istringstream istr (line);
  int v1;
  istr >> v1;
  int v2;
  istr >> v2;
  if (istr.fail ())
    throw std::runtime_error ("Failed to parse " + line + " as an Edge.");
  return Edge (v1, v2);
}

void
load (istream &input, Graph *gr)
{
  int n = mygetnumber (input);
  // cout << n << endl;
  gr->v.clear ();
  gr->e.clear ();
  for (int i = 0; i < n; i++)
    gr->v.push_back (i);

  gr->p = mygetnumber (input);
  // cout << gr->p << endl;

  for (int i = 0; i < n; i++)
    mygetnumber (input);  // ignore the initial ordering
  while (true)
    {
      try
	{
	  gr->e.push_back (mygetedge (input));
	}
      catch (NoMoreLinesException &exc)
	{
	  break;
	}
      catch (std::exception &exc)
	{
	  throw;
	}
    }
  cerr << gr->e.size () << endl;
  // Set the pointers to edges after the vector of all edges is filled and will not be moved.
  gr->updateNeighs ();
}
