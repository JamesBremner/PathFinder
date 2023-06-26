///  graph with vertices on a 2D grid
class cGrid2D
{
public:
    typedef std::pair<int, int> point_t;
    typedef std::pair<point_t, point_t> edge_t;
    typedef std::vector<point_t> path_t;

    cGrid2D()
    {
        reset();
    }
    void reset()
    {
        myColCount = -1;
        myRowCount = -1;
    }

    /// @brief Extend grid dimensions if neccessary to include point
    /// @param x zero based col
    /// @param y zero based row

    void extDim(int x, int y)
    {
        if (x < 0 || y < 0)
            throw std::runtime_error(
                "Bad cGrid2D co-ords");
        if (x > myColCount - 1)
            myColCount = x + 1;
        if (y > myRowCount - 1)
            myRowCount = y + 1;
    }

    /// @brief set grid dimensions
    /// @param colCount
    /// @param rowCount

    void setDim(int colCount, int rowCount)
    {
        myColCount = colCount;
        myRowCount = rowCount;
    }

    /// @brief Add edge between vertices
    /// @param src vertex index
    /// @param dst vertex index
    void addEdge(int src, int dst)
    {
        vEdge.push_back(std::make_pair(src, dst));
    }

    /// @brief get grid dimensions
    /// @param[out] colCount
    /// @param[out] rowCount

    void getDim(int &colCount, int &rowCount) const
    {
        colCount = myColCount;
        rowCount = myRowCount;
    }

    /// @brief get vertex index from location
    /// @param col
    /// @param row
    /// @return vertex index, or -1

    int index(int col, int row) const
    {
        if (0 > row || row >= myRowCount ||
            0 > col || col >= myColCount)
            return -1;
        return row * myColCount + col;
    }

    /// @brief get vertex location from index
    /// @param[out] col
    /// @param[out] row
    /// @param index

    void coords(int &col, int &row, int index) const
    {
        row = index / myColCount;
        col = index - row * myColCount;
    }

    /// @brief get point locations
    /// @return vector of point locations

    std::vector<point_t>
    getPoints() const
    {
        std::vector<std::pair<int, int>> ret;
        int x, y;
        for (int p = 0; p < myColCount * myRowCount; p++)
        {
            coords(x, y, p);
            ret.push_back(std::make_pair(x, y));
        }
        return ret;
    }

    std::vector<int>
    getOrthoNeighboursRightBelow(int cell)
    {
        std::vector<int> ret;
        int col, row;
        coords(col, row, cell);
        int ncol, nrow;
        ncol = col + 1;
        nrow = row;
        if (ncol < myColCount && nrow < myRowCount)
            ret.push_back(index(ncol, nrow));
        ncol = col;
        nrow = row + 1;
        if (ncol < myColCount && nrow < myRowCount)
            ret.push_back(index(ncol, nrow));
        ncol = col + 1;
        nrow = row + 1;
        if (ncol < myColCount && nrow < myRowCount)
            ret.push_back(index(ncol, nrow));
        return ret;
    }

    std::vector<int>
    getOrthoDiagNeighboursRightBelow(int cell)
    {
        std::vector<int> ret;
        int col, row;
        coords(col, row, cell);
        ret = getOrthoNeighboursRightBelow(cell);
        int ncol = col + 1;
        int nrow = row + 1;
        if (ncol < myColCount && nrow < myRowCount)
            ret.push_back(index(ncol, nrow));
        return ret;
    }

    /// @brief get edge locations
    /// @return vector of edge locations (src col,src row), (dst col, dst row )

    std::vector<edge_t>
    getEdges() const
    {
        std::vector<edge_t> ret;
        for (auto &e : vEdge)
        {
            int x1, y1, x2, y2;
            coords(x1, y1, e.first);
            coords(x2, y2, e.second);
            ret.push_back(
                std::make_pair(
                    std::make_pair(x1, y1), std::make_pair(x2, y2)));
        }
        return ret;
    }

    std::string name( int cell ) const
    {
        int c,r;
        coords(c,r,cell);
        if (0 > c || c >= myColCount &&
            0 > r && r >= myRowCount )
            return "";
        return "c" + std::to_string(c) + "r" + std::to_string(r);
    }

    /// @brief get edge indices
    /// @return vector of index pairs (v1 index, v2 index )
    std::vector<std::pair<int, int>>
    getEdgesVertexIndex() const
    {
        return vEdge;
    }

    /// @brief Manhatten path, horizontal part
    /// @param s starting point
    /// @param d destination point
    /// @return path, including start
    static path_t
    X(const point_t &s, const point_t &d)
    {
        path_t ret;
        ret.push_back(s);
        int deltax = d.first - s.first;
        if (deltax)
        {
            int inc = 1;
            if (deltax < 0)
                inc = -1;
            int x = s.first;
            do
            {
                x += inc;
                ret.push_back(std::make_pair(x, s.second));
            } while (x != d.first);
        }
        return ret;
    }

    /// @brief Manhatten path, vertical part
    static path_t
    Y(const point_t &s, const point_t &d)
    {
        std::vector<std::pair<int, int>> ret;
        ret.push_back(s);
        int deltay = d.second - s.second;
        if (deltay)
        {
            int inc = 1;
            if (deltay < 0)
                inc = -1;
            int y = s.second;
            do
            {
                y += inc;
                ret.push_back(std::make_pair(s.first, y));
            } while (y != d.second);
        }
        return ret;
    }

    /// @brief Manhatten path, diagonal steps
    static path_t
    D(const point_t &s, const point_t &d)
    {
        std::vector<std::pair<int, int>> ret;
        ret.push_back(s);
        int deltax = d.first - s.first;
        int deltay = d.second - s.second;
        int incx = 1;
        int incy = 1;
        if (deltax < 0)
            incx = -1;
        if (deltay < 0)
            incy = -1;
        int x = s.first;
        int y = s.second;
        while ((x != d.first) || (y != d.second))
        {
            if (x == d.first)
                incx = 0;
            if (y == d.second)
                incy = 0;
            x += incx;
            y += incy;
            ret.push_back(std::make_pair(x, y));
        }
        return ret;
    }

    /// @brief Manhatten path, vertical than horizontal
    static path_t
    YX(const point_t &s, const point_t &d)
    {
        auto path = Y(s, d);
        auto path2 = X(point_t(s.first, d.second), d);
        path.insert(
            path.end(),
            path2.begin() + 1,
            path2.end());
        return path;
    }

    /// @brief Manhatten path, horizontal then vertical
    static path_t
    XY(const point_t &s, const point_t &d)
    {
        auto path = X(s, d);
        auto path2 = Y(point_t(d.first, s.second), d);
        path.insert(
            path.end(),
            path2.begin() + 1,
            path2.end());
        return path;
    }

private:
    int myColCount;
    int myRowCount;
    std::vector<std::pair<int, int>> vEdge;
};
