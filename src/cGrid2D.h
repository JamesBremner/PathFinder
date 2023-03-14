///  graph with vertices on a 2D grid
class cGrid2D
{
public:
    typedef std::pair<int,int>  point_t;
    typedef std::pair<point_t,point_t> edge_t;

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
        if (x > myColCount-1)
            myColCount = x+1;
        if (y > myRowCount-1)
            myRowCount = y+1;
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
    /// @return vertex index

    int index(int col, int row) const
    {
        if( 0 > row || row > myRowCount-1 ||
            0 > col || col > myColCount )
            throw std::runtime_error(
                "cGrid2d::index bad col,row"            );
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

    /// @brief get edge locations
    /// @return vector of edge locations (src col,src row), (dst col, dst row )

    std::vector<edge_t>
    getEdges() const
    {
        std::vector<edge_t> ret;
        for( auto& e : vEdge )
        {
            int x1,y1,x2,y2;
            coords(x1,y1,e.first);
            coords(x2,y2,e.second);
            ret.push_back(
                std::make_pair(
                    std::make_pair(x1,y1),std::make_pair(x2,y2)));
        }
        return ret;
    }

    private:
    int myColCount;
    int myRowCount;
    std::vector<std::pair<int,int>> vEdge;
};
