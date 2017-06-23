////////////////////////////////////////////////////////////////////
//
// Dean Andrew Hidas <dhidas@bnl.gov>
//
// Created on: Wed May 31 08:40:08 EDT 2017
//
// This is a namespace for common c-python functions used throughout
// the oscars c code.
//
////////////////////////////////////////////////////////////////////
#include "Python.h"

#include "OSCARSPY.h"

#include <stdexcept>

namespace OSCARSPY {

std::string GetVersionString ()
{
  // Get the version string based off of the compiler defines
  char ver[200];
  if (OSCARS_RELEASE == NULL) {
    sprintf(ver, "%i.%i.%i", OSCARS_VMAJOR, OSCARS_VMINOR, OSCARS_REVISION);
  } else {
    sprintf(ver, "%i.%i.%i.%s", OSCARS_VMAJOR, OSCARS_VMINOR, OSCARS_REVISION, OSCARS_RELEASE);
  }
  return std::string(ver);
}




char* GetAsString (PyObject* S)
{
  // Return the correct string version depending on the python version

  #if PY_MAJOR_VERSION >= 3
  return PyUnicode_AsUTF8(S);
  #else
  return PyString_AsString(S);
  #endif
}



char* GetVersionOfModule (std::string const& ModuleName)
{
  PyObject* pkg_resources = PyImport_ImportModule("pkg_resources");
  if (pkg_resources == NULL) {
    throw std::invalid_argument("cannot import pkg_resources");
  }
  PyObject* dist = PyObject_CallMethod(pkg_resources, "get_distribution", "s", ModuleName.c_str());
  if (dist == NULL) {
    throw std::invalid_argument("cannot call get_distribution with this argument");
  }
  PyObject* ver = PyObject_GetAttrString(dist, "version");
  if (ver == NULL) {
    throw std::invalid_argument("cannot find version");
  }

  return GetAsString(ver);
}




PyObject* GetSpectrumAsList (TSpectrumContainer const& Spectrum)
{
  // Get the spectrum as a list format for python output

  // Create a python list
  PyObject *PList = PyList_New(0);

  // Number of points in trajectory calculation
  size_t NSPoints = Spectrum.GetNPoints();

  // Loop over all points in trajectory
  for (size_t iS = 0; iS != NSPoints; ++iS) {
    // Create a python list for X and Beta
    PyObject *PList2 = PyList_New(0);

    // Add position and Beta to list
    PyList_Append(PList2, Py_BuildValue("f", Spectrum.GetEnergy(iS)));
    PyList_Append(PList2, Py_BuildValue("f", Spectrum.GetFlux(iS)));
    PyList_Append(PList, PList2);
  }

  // Return the python list
  return PList;
}




TSpectrumContainer GetSpectrumFromList (PyObject* List)
{
  // Take an input list in spectrum format and convert it to TSpectrumContainer object

  // Increment reference for list
  Py_INCREF(List);

  // Get size of input list
  size_t const NPoints = PyList_Size(List);
  if (NPoints <= 0) {
    throw;
  }

  TSpectrumContainer S;

  for (size_t ip = 0; ip != NPoints; ++ip) {
    PyObject* List_Point = PyList_GetItem(List, ip);
    if (PyList_Size(List_Point) == 2) {
      S.AddPoint(PyFloat_AsDouble(PyList_GetItem(List_Point, 0)), PyFloat_AsDouble(PyList_GetItem(List_Point, 1)));
    } else {
      throw;
    }
  }


  // Increment reference for list
  Py_DECREF(List);

  // Return the object
  return S;
}







TVector2D ListAsTVector2D (PyObject* List)
{
  // Get a list as a TVector2D

  TVector2D V;
  if (PyList_Size(List) == 2) {
    Py_INCREF(List);
    V.SetXY(PyFloat_AsDouble(PyList_GetItem(List, 0)),
             PyFloat_AsDouble(PyList_GetItem(List, 1)));
    Py_DECREF(List);
  } else {
    throw std::length_error("number of elements not 2");
  }

  // Return the python list
  return V;
}




TVector3D ListAsTVector3D (PyObject* List)
{
  // Get a list as a TVector3D

  TVector3D V;
  if (PyList_Size(List) == 3) {
    Py_INCREF(List);
    V.SetXYZ(PyFloat_AsDouble(PyList_GetItem(List, 0)),
             PyFloat_AsDouble(PyList_GetItem(List, 1)),
             PyFloat_AsDouble(PyList_GetItem(List, 2)));
    Py_DECREF(List);
  } else {
    throw std::length_error("number of elements not 3");
  }

  // Return the python list
  return V;
}




PyObject* TVector2DAsList (TVector2D const& V)
{
  // Turn a TVector2D into a list (like a vector)

  // Create a python list
  PyObject *PList = PyList_New(0);

  PyList_Append(PList, Py_BuildValue("f", V.GetX()));
  PyList_Append(PList, Py_BuildValue("f", V.GetY()));

  // Return the python list
  return PList;
}




PyObject* TVector3DAsList (TVector3D const& V)
{
  // Turn a TVector3D into a list (like a vector)

  // Create a python list
  PyObject *PList = PyList_New(0);

  PyList_Append(PList, Py_BuildValue("f", V.GetX()));
  PyList_Append(PList, Py_BuildValue("f", V.GetY()));
  PyList_Append(PList, Py_BuildValue("f", V.GetZ()));

  // Return the python list
  return PList;
}







T3DScalarContainer GetT3DScalarContainerFromList (PyObject* List)
{
  // Take an input list and convert it to T3DScalarContainer object

  // Increment reference for list
  Py_INCREF(List);

  // Get size of input list
  size_t const NPoints = PyList_Size(List);
  if (NPoints <= 0) {
    throw;
  }

  T3DScalarContainer F;

  for (size_t ip = 0; ip != NPoints; ++ip) {
    PyObject* List_Point = PyList_GetItem(List, ip);
    if (PyList_Size(List_Point) == 2) {
      F.AddPoint(OSCARSPY::ListAsTVector3D(PyList_GetItem(List_Point, 0)), PyFloat_AsDouble(PyList_GetItem(List_Point, 1)));
    } else {
      throw;
    }
  }


  // Increment reference for list
  Py_DECREF(List);

  // Return the object
  return F;
}










}
