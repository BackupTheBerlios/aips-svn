#ifdef USE_STANDALONE
/**
 * \param value value to be rounded
 * \return rounded value
 */
template<typename T> inline T round( const T value ) throw()
{
  return static_cast<T>( floor( value + 0.5f ) );
}

/**
 * \param aVector vector to compute norm from
 * \return Norm of given vector
 */
template<typename T> inline double norm( const T& aVector )
{
  return sqrt( dot( aVector, aVector ) );
}
#endif /* #ifdef USE_STANDALONE */

/**
 * This is faster than a simple norm function since sqrt() is not used.
 * Prefer this function if you need to compare two vectors,
 * but are not interested in the actual values.
 * \param aVector vector to compute norm from
 * \return Norm^2 of given vector
 */
template<typename T> inline double quadNorm( const T& aVector ) throw()
{
  return dot( aVector, aVector );
}

/**
 * \param leftVector first Vector for product
 * \param rightVector second Vector for product
 */
inline TVector3D vecProd( const TVector3D& leftVector, const TVector3D& rightVector ) throw()
{
  return TVector3D( leftVector[1] * rightVector[2] - leftVector[2] * rightVector[1],
    leftVector[2] * rightVector[0] - leftVector[0] * rightVector[2],
    leftVector[0] * rightVector[1] - leftVector[1] * rightVector[0] );
}

